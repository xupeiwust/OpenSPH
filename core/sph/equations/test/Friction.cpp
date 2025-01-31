#include "sph/equations/Friction.h"
#include "catch.hpp"
#include "objects/geometry/Domain.h"
#include "sph/initial/Initial.h"
#include "sph/solvers/SymmetricSolver.h"
#include "system/Statistics.h"
#include "tests/Approx.h"
#include "thread/Pool.h"
#include "utils/SequenceTest.h"

using namespace Sph;

TEST_CASE("InternalFriction", "[friction]") {
    EquationHolder eqs;
    RunSettings settings;
    settings.set(RunSettingsId::SPH_SOLVER_FORCES, ForceEnum::PRESSURE);
    eqs +=
        makeTerm<ViscousStress>() + makeTerm<ContinuityEquation>(settings) + makeTerm<ConstSmoothingLength>();

    ThreadPool& pool = *ThreadPool::getGlobalInstance();
    SymmetricSolver<3> solver(pool, settings, std::move(eqs));

    Storage storage;
    InitialConditions initial(RunSettings::getDefaults());
    BodySettings body;
    body.set(BodySettingsId::RHEOLOGY_YIELDING, YieldingEnum::NONE);
    body.set(BodySettingsId::RHEOLOGY_DAMAGE, FractureEnum::NONE);
    body.set(BodySettingsId::EOS, EosEnum::NONE);
    body.set(BodySettingsId::PARTICLE_COUNT, 10000);
    initial.addMonolithicBody(storage, BlockDomain(Vector(0._f), Vector(2._f, 2._f, 1._f)), body);
    solver.create(storage, storage.getMaterial(0));

    /// \todo this is normally createdb by rheology, but we actually don't need any rheology here. Better
    /// solution?
    storage.insert<Float>(QuantityId::STRESS_REDUCING, OrderEnum::ZERO, 1._f);

    // add two sliding layers
    ArrayView<Vector> r, v, dv;
    tie(r, v, dv) = storage.getAll<Vector>(QuantityId::POSITION);
    for (Size i = 0; i < r.size(); ++i) {
        if (r[i][Z] > 0._f) {
            v[i] = Vector(10._f, 0._f, 0._f);
        }
    }
    Statistics stats;
    /// \todo this is currently necessary as the friction depends on pre-computed grad-v  :(
    solver.integrate(storage, stats);
    solver.integrate(storage, stats);
    ArrayView<const Size> neighs = storage.getValue<Size>(QuantityId::NEIGHBOR_CNT);
    tie(r, v, dv) = storage.getAll<Vector>(QuantityId::POSITION);
    const Float h = r[0][H];
    auto test = [&](const Size i) -> Outcome {
        if (max(abs(r[i][X]), abs(r[i][Y]), abs(r[i][Z])) > 0.8_f) {
            // skip boundary particles
            return SUCCESS;
        }
        if (Interval(0._f, h).contains(r[i][Z])) {
            // these particles should be slowed down
            if (dv[i][X] >= -1.e-5_f) {
                // clang-format off
                return makeFailed("Friction didn't decelerate:\n dv == {}\n r == {}\n v == {}\n neigh cnt == {}",
                                  dv[i], r[i], v[i], neighs[i]);
                // clang-format on
            }
            return SUCCESS;
        }
        if (Interval(h, 1._f).contains(r[i][Z])) {
            // these should either be slowed down or remain unaffected
            if (dv[i][X] > 0._f) {
                return makeFailed("Friction accelerated where is shouldn't\ndv == {}", dv[i]);
            }
            return SUCCESS;
        }
        if (Interval(-h, 0._f).contains(r[i][Z])) {
            // these particles should be accelerated in X
            if (dv[i][X] <= 1.e-5_f) {
                // clang-format off
                return makeFailed("Friction didn't accelerate:\n dv == {}\n r == {}\n v == {}\n neigh cnt == {}",
                                   dv[i], r[i], v[i], neighs[i]);
                // clang-format on
            }
            return SUCCESS;
        }
        if (Interval(-1._f, -h).contains(r[i][Z])) {
            // these should either be accelerated or remain uaffected
            if (dv[i][X] < 0._f) {
                return makeFailed("Friction decelerated where is shouldn't.\n dv == {}\n r == {}\n v == {}",
                    dv[i],
                    r[i],
                    v[i]);
            }
            return SUCCESS;
        }
        STOP; // sanity check that we checked all particles
    };
    REQUIRE_SEQUENCE(test, 0, r.size());
}
