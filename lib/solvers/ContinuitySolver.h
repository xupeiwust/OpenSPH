#pragma once

/// Standard SPH solver, using density and specific energy as independent variables. Evolves density using
/// continuity equation and energy using energy equation. Works with any artificial viscosity and any equation
/// of state.
/// Pavel Sevecek 2016
/// sevecek at sirrah.troja.mff.cuni.cz

#include "objects/containers/ArrayUtils.h"
#include "objects/finders/Finder.h"
#include "solvers/AbstractSolver.h"
#include "sph/av/Standard.h"
#include "sph/boundary/Boundary.h"
#include "sph/forces/StressForce.h"
#include "sph/kernel/Kernel.h"
#include "storage/Iterate.h"
#include "storage/QuantityKey.h"
#include "storage/QuantityMap.h"
#include "system/Factory.h"
#include "system/Profiler.h"
#include "system/Settings.h"

NAMESPACE_SPH_BEGIN

template <typename Force, int D>
class ContinuitySolver : public SolverBase<D> {
private:
    Force force;

    static constexpr int dim = D;

    /// \todo SharedAccumulator
    Accumulator<RhoDivv> rhoDivv;

public:
    ContinuitySolver(const GlobalSettings& settings)
        : SolverBase<D>(settings)
        , force(settings)
    {}

    virtual void integrate(Storage& storage) override {
        const int size = storage.getParticleCnt();

        ArrayView<Vector> r, v, dv;
        tieToArray(r, v, dv) = storage.getAll<Vector>(QuantityKey::R);
        ArrayView<Float> m, rho, drho;
        tieToArray(rho, drho) = storage.getAll<Float>(QuantityKey::RHO);
        m = storage.getValue<Float>(QuantityKey::M);
        // Check that quantities are valid
        ASSERT(areAllMatching(dv, [](const Vector v) { return v == Vector(0._f); }));
        ASSERT(areAllMatching(rho, [](const Float v) { return v > 0._f; }));

        // clamp smoothing length
        for (Float& h : componentAdapter(r, H)) {
            h = Math::max(h, 1.e-12_f);
        }
        // initialize stuff
        rhoDivv.update(storage);
        force.update(storage);

        // find new pressure
        /// \todo update pressure and sound speed
        // this->computeMaterial(storage);

        /*void Abstract::Solver::computeMaterial(Storage& storage) {
            ArrayView<Float> p, rho, u, cs;
            tieToArray(p, cs, rho, u) =
                storage.getValues<Float>(QuantityKey::P, QuantityKey::CS, QuantityKey::RHO, QuantityKey::U);

            for (int i = 0; i < storage.getParticleCnt(); ++i) {
                Material& mat = storage.getMaterial(i);
                tieToTuple(p[i], cs[i]) = mat.eos->getPressure(rho[i], u[i]);
            }
            ASSERT(areAllMatching(rho, [](const Float v) { return v > 0._f; }));
        }*/

        // build neighbour finding object
        /// \todo only rebuild(), try to limit allocations
        PROFILE_SCOPE("ContinuitySolver::compute (init)")
        this->finder->build(r);

        // we symmetrize kernel by averaging smoothing lenghts
        SymH<dim> w(this->kernel);

        PROFILE_NEXT("ContinuitySolver::compute (main cycle)")
        for (int i = 0; i < size; ++i) {
            // Find all neighbours within kernel support. Since we are only searching for particles with
            // smaller h, we know that symmetrized lengths (h_i + h_j)/2 will be ALWAYS smaller or equal to
            // h_i, and we thus never "miss" a particle.
            this->finder->findNeighbours(
                i, r[i][H] * this->kernel.radius(), this->neighs, FinderFlags::FIND_ONLY_SMALLER_H);
            // iterate over neighbours
            for (const auto& neigh : this->neighs) {
                const int j = neigh.index;
                // actual smoothing length
                const Float hbar = 0.5_f * (r[i][H] + r[j][H]);
                ASSERT(hbar > EPS && hbar <= r[i][H]);
                if (getSqrLength(r[i] - r[j]) > Math::sqr(this->kernel.radius() * hbar)) {
                    // aren't actual neighbours
                    continue;
                }
                // compute gradient of kernel W_ij
                const Vector grad = w.grad(r[i], r[j]);
                ASSERT(dot(grad, r[i] - r[j]) <= 0._f);

                force.accumulate(i, j, grad);
                rhoDivv.accumulate(i, j, grad);
            }
        }

        // set derivative of density and smoothing length
        for (int i = 0; i < drho.size(); ++i) {
            drho[i] = -rhoDivv[i];
            /// \todo smoothing length
            v[i][H] = 0._f;
            dv[i][H] = 0._f;
        }
        force.evaluate(storage);
        if (this->boundary) {
            this->boundary->apply(storage);
        }
    }

    virtual QuantityMap getQuantityMap() const override {
        QuantityMap map;
        map[QuantityKey::RHO] = { ValueEnum::SCALAR, OrderEnum::FIRST_ORDER };
        map[QuantityKey::U] = { ValueEnum::SCALAR, OrderEnum::FIRST_ORDER };
        map[QuantityKey::M] = { ValueEnum::SCALAR, OrderEnum::ZERO_ORDER };
        map.add(force.template getQuantityMap());
        return map;
    }
};

NAMESPACE_SPH_END
