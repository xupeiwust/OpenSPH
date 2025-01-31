/// \file CliffCollapse.cpp
/// \brief Cliff collapse test
/// \author Pavel Sevecek (sevecek at sirrah.troja.mff.cuni.cz)
/// \date 2016-2021

#include "Common.h"
#include "catch.hpp"
#include "sph/equations/Potentials.h"

using namespace Sph;

class CliffCollapse : public IRun {
private:
    // scale of the experiment; should only affect timestep.
    static constexpr Float SCALE = 1.e3_f;

public:
    CliffCollapse() {
        settings.set(RunSettingsId::RUN_NAME, String("Cliff Collapse Problem"))
            .set(RunSettingsId::TIMESTEPPING_INTEGRATOR, TimesteppingEnum::PREDICTOR_CORRECTOR)
            .set(RunSettingsId::TIMESTEPPING_INITIAL_TIMESTEP, 1.e-8_f)
            .set(RunSettingsId::TIMESTEPPING_MAX_TIMESTEP, 100._f)
            .set(RunSettingsId::RUN_END_TIME, 200._f)
            .set(RunSettingsId::RUN_OUTPUT_TYPE, IoEnum::BINARY_FILE)
            .set(RunSettingsId::RUN_OUTPUT_INTERVAL, 2000._f)
            .set(RunSettingsId::RUN_OUTPUT_PATH, String("cliff_collapse"))
            .set(RunSettingsId::RUN_OUTPUT_NAME, String("cliff_%d.ssf"))
            .set(RunSettingsId::SPH_SOLVER_FORCES, ForceEnum::PRESSURE | ForceEnum::SOLID_STRESS)
            .set(RunSettingsId::SPH_SOLVER_TYPE, SolverEnum::ASYMMETRIC_SOLVER)
            .set(RunSettingsId::SPH_FINDER, FinderEnum::KD_TREE)
            .set(RunSettingsId::SPH_DISCRETIZATION, DiscretizationEnum::STANDARD)
            .set(RunSettingsId::SPH_AV_TYPE, ArtificialViscosityEnum::STANDARD)
            .set(RunSettingsId::SPH_AV_USE_STRESS, false)
            .set(RunSettingsId::SPH_AV_ALPHA, 1.5_f)
            .set(RunSettingsId::SPH_AV_BETA, 3._f)
            .set(RunSettingsId::SPH_KERNEL, KernelEnum::CUBIC_SPLINE)
            .set(RunSettingsId::TIMESTEPPING_DERIVATIVE_FACTOR, 0.2_f)
            .set(RunSettingsId::TIMESTEPPING_COURANT_NUMBER, 0.2_f)
            .set(RunSettingsId::RUN_THREAD_GRANULARITY, 100)
            .set(RunSettingsId::SPH_ADAPTIVE_SMOOTHING_LENGTH, EMPTY_FLAGS)
            .set(RunSettingsId::SPH_STRAIN_RATE_CORRECTION_TENSOR, true)
            .set(RunSettingsId::SPH_SUM_ONLY_UNDAMAGED, false)
            .set(RunSettingsId::DOMAIN_BOUNDARY, BoundaryEnum::GHOST_PARTICLES)
            .set(RunSettingsId::DOMAIN_GHOST_MIN_DIST, 0.5_f)
            .set(RunSettingsId::DOMAIN_TYPE, DomainEnum::BLOCK)
            .set(RunSettingsId::DOMAIN_CENTER, Vector(3._f, 3._f, 0._f) * SCALE)
            .set(RunSettingsId::DOMAIN_SIZE, Vector(6.01_f, 6.01_f, 3.01_f) * SCALE);

        scheduler = Factory::getScheduler(settings);
    }

    virtual void setUp(SharedPtr<Storage> storage) override {
        Size N = 10000;

        BodySettings body;
        body.set(BodySettingsId::ENERGY, 10._f)
            .set(BodySettingsId::ENERGY_RANGE, Interval(0._f, INFTY))
            .set(BodySettingsId::MELT_ENERGY, 1.e12_f)
            .set(BodySettingsId::EOS, EosEnum::TILLOTSON)
            .set(BodySettingsId::RHEOLOGY_DAMAGE, FractureEnum::SCALAR_GRADY_KIPP)
            .set(BodySettingsId::RHEOLOGY_YIELDING, YieldingEnum::DRUCKER_PRAGER)
            .set(BodySettingsId::SHEAR_MODULUS, 1.e9_f)
            .set(BodySettingsId::DISTRIBUTE_MODE_SPH5, false)
            .set(BodySettingsId::INITIAL_DISTRIBUTION, DistributionEnum::HEXAGONAL)
            .set(BodySettingsId::DRY_FRICTION, 0.8_f)
            .set(BodySettingsId::ENERGY_MIN, 1000._f)
            .set(BodySettingsId::STRESS_TENSOR_MIN, 1.e10_f)
            .set(BodySettingsId::DAMAGE, 1._f)
            .set(BodySettingsId::DAMAGE_MIN, 10._f)
            .set(BodySettingsId::PARTICLE_COUNT, int(N));

        EquationHolder eqs = getStandardEquations(settings);
        eqs += makeExternalForce([](const Vector UNUSED(r), const Float UNUSED(t)) { //
            return Vector(0._f, -9.81_f, 0._f);
        });

        AutoPtr<IDomain> boundary = Factory::getDomain(settings);
        AutoPtr<GhostParticles> bc = makeAuto<GhostParticles>(std::move(boundary), settings);
        bc->setVelocityOverride([](const Vector& r) -> Optional<Vector> {
            if (r[Y] < 1.e3_f) {
                return Vector(0._f); // zero velocity (=> friction)
            } else {
                return NOTHING;
            }
        });

        solver = makeAuto<AsymmetricSolver>(*scheduler, settings, eqs, std::move(bc));

        const Vector dimension = Vector(1._f, 3.2_f, 3._f) * SCALE;
        BlockDomain block(0.5_f * Vector(dimension[X], dimension[Y], 0._f), dimension);
        InitialConditions ic(settings);
        ic.addMonolithicBody(*storage, block, body);
        solver->create(*storage, storage->getMaterial(0));

        logWriter = makeAuto<NullLogWriter>();

        /// \todo change to logfile
        triggers.pushBack(makeAuto<ProgressLog>(10._f));
    }

protected:
    virtual void tearDown(const Storage& storage, const Statistics& stats) override {
        output->dump(storage, stats);
    }
};

TEST_CASE("Cliff Collapse", "[rheology]") {
    Array<Path> filesToCheck = { Path("cliff_collapse/cliff_0000.ssf"),
        Path("cliff_collapse/cliff_0001.ssf") };

    for (Path file : filesToCheck) {
        FileSystem::removePath(file);
    }

    measureRun(Path("cliff_collapse/stats"), [] {
        CliffCollapse run;
        Storage storage;
        run.run(storage);
    });

    for (Path file : filesToCheck) {
        REQUIRE(areFilesApproxEqual(file, REFERENCE_DIR / file.fileName()) == SUCCESS);
    }
}
