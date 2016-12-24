#pragma once

#include "objects/Object.h"
#include "sph/kernel/Kernel.h"
#include "storage/Storage.h"
#include "system/Factory.h"
#include "system/Profiler.h"
#include "system/Settings.h"
#include <memory>

NAMESPACE_SPH_BEGIN

namespace Abstract {
    class Domain;
    class Finder;
    class BoundaryConditions;
}
struct NeighbourRecord;
class QuantityMap;

namespace Abstract {
    class Solver : public Polymorphic {
    public:
        /// Computes derivatives of all time-dependent quantities.
        /// \param storage Storage containing all quantities. All highest order derivatives are guaranteed to
        ///        be set to zero (this is responsibility of TimeStepping).
        virtual void integrate(Storage& storage) = 0;

        /// Returns the quantity map used by the solver.
        virtual QuantityMap getQuantityMap() const = 0;
    };
}

/// Extended base class for solvers (templated class, cannot be used directly in methods as templated virtual
/// methods do not exist)
template <int D>
class SolverBase : public Abstract::Solver {
protected:
    std::unique_ptr<Abstract::Finder> finder;

    Array<NeighbourRecord> neighs;

    std::unique_ptr<Abstract::BoundaryConditions> boundary;

    static constexpr int dim = D;
    LutKernel<D> kernel;

public:
    SolverBase(const GlobalSettings& settings) {
        finder = Factory::getFinder(settings);
        kernel = Factory::getKernel<D>(settings);

        std::unique_ptr<Abstract::Domain> domain = Factory::getDomain(settings);
        boundary = Factory::getBoundaryConditions(settings, std::move(domain));
    }
};

NAMESPACE_SPH_END
