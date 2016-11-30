#pragma once

/// Definition of physical model.
/// Pavel Sevecek 2016
/// sevecek at sirrah.troja.mff.cuni.cz

#include "objects/Object.h"
#include "solvers/AbstractSolver.h"
#include "sph/av/Standard.h"
#include "sph/boundary/Boundary.h"
#include "sph/kernel/Kernel.h"
#include "storage/QuantityKey.h"
#include "system/Settings.h"


NAMESPACE_SPH_BEGIN

namespace Abstract {
    class Finder;
    class Eos;
}
struct NeighbourRecord;

/// Uses density and specific energy as independent variables. Density is solved by direct summation, using
/// self-consistent solution with smoothing length. Energy is evolved using energy equation.
template <int d>
class SummationSolver : public Abstract::Solver {
private:
    std::unique_ptr<Abstract::Finder> finder;

    std::unique_ptr<Abstract::Eos> eos;

    std::unique_ptr<Abstract::BoundaryConditions> boundary;

    /// \todo what if we have more EoSs in one model? (rock and ice together in a comet)
    LutKernel<d> kernel;
    Array<NeighbourRecord> neighs; /// \todo store neighbours directly here?!


    Array<Float> accumulatedRho;
    Array<Float> accumulatedH;

    Float eta;

    StandardAV monaghanAv;


public:
    SummationSolver(const std::shared_ptr<Storage>& storage, const Settings<GlobalSettingsIds>& settings);

    ~SummationSolver(); // necessary because of unique_ptrs to incomplete types

    virtual void compute(Storage& storage) override;

    virtual Storage createParticles(const Abstract::Domain& domain,
                                    const Settings<BodySettingsIds>& settings) const override;
};

NAMESPACE_SPH_END
