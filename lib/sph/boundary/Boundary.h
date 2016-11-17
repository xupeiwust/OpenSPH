#pragma once

/// Boundary conditions
/// Pavel Sevecek 2016
/// sevecek at sirrah.troja.mff.cuni.cz

#include "geometry/Domain.h"
#include "objects/wrappers/Range.h"
#include "storage/Storage.h"
#include <memory>

/// \todo create some reasonable interface

NAMESPACE_SPH_BEGIN

namespace Abstract {
    class BoundaryConditions : public Polymorphic {
    public:
        /// Applies boundary conditions onto the particles
        virtual void apply() = 0;
    };
}

/// Add ghost particles symmetrically for each SPH particle close to boundary, copying all quantities to them.
class GhostParticles : public Abstract::BoundaryConditions {
private:
    // indices into storage marking ghost particles. MUST always be after regular particles
    Array<int> indices;

public:
    /*beforeStep
        add particles

    afterStep
        nothing*/
};

enum class ProjectingOptions {
    ZERO_VELOCITY,      ///< velocities of particles outside of domain are set to zero
    ZERO_PERPENDICULAR, /// < sets perpendicular component of the velocity to zero, parallel remains the same
    REFLECT,            ///< particles 'bounce off' the boundary, the perpendicular component of the velocity
                        ///  changes sign
};

/// Simply project all particles outside of the domain to its boundary.
class DomainProjecting : public Abstract::BoundaryConditions {
private:
    std::shared_ptr<Storage> storage;
    std::unique_ptr<Abstract::Domain> domain;
    Array<int> outside;
    Array<Vector> vproj;
    ProjectingOptions options;

public:
    DomainProjecting(const std::shared_ptr<Storage>& storage,
                     std::unique_ptr<Abstract::Domain>&& domain,
                     const ProjectingOptions options)
        : storage(storage)
        , domain(std::move(domain))
        , options(options) {}

    virtual void apply() override {
        Array<Vector>& r = this->storage->get<QuantityKey::R>();
        Array<Vector>& v = this->storage->dt<QuantityKey::R>();
        // check which particles are outside of the domain
        domain->getSubset(r, outside, SubsetType::OUTSIDE);
        domain->project(r, outside);
        vproj.clear();
        int idx = 0;
        switch (options) {
        case ProjectingOptions::ZERO_VELOCITY:
            for (int i : outside) {
                v[i] = Vector(0._f);
            }
            break;
        case ProjectingOptions::ZERO_PERPENDICULAR:
            projectVelocity(r, v);
            for (int i : outside) {
                v[i] = 0.5_f * (v[i] + vproj[idx] - r[i]);
            }
            break;
        case ProjectingOptions::REFLECT:
            projectVelocity(r, v);
            for (int i : outside) {
                // subtract the original position and we have projected velocities! Yay!)
                v[i] = vproj[idx] - r[i];
            }
            break;
        }
    }

private:
    void projectVelocity(ArrayView<const Vector> r, ArrayView<const Vector> v) {
        /// \todo implement using normal to the boundary
        for (int i : outside) {
            // sum up position and velocity
            vproj.push(r[i] + v[i]);
        }
        // invert the sum
        domain->invert(vproj);
    }
};

class PeriodicDomain : public Abstract::BoundaryConditions {
    /// \todo modify Finder to search periodically in domain. That should be the whole trick
};

class Projection1D : public Abstract::BoundaryConditions {
private:
    std::shared_ptr<Storage> storage;
    Range<Float> domain;

public:
    /// Constructs using range as 1D domain
    Projection1D(const std::shared_ptr<Storage>& storage, const Range<Float> domain)
        : storage(storage)
        , domain(domain) {}

    virtual void apply() override {
        Array<Vector>& r = this->storage->get<QuantityKey::R>();
        Array<Vector>& v = this->storage->dt<QuantityKey::R>();

        for (int i = 0; i < r.size(); ++i) {
            // throw away y and z, keep h
            r[i] = Vector(domain.clamp(r[i][0]), 0._f, 0._f, r[i][H]);
            v[i] = Vector(v[i][0], 0._f, 0._f);
        }
        // null velocity of first and last particle
        v[0] = Vector(0._f);
        v[v.size()-1] = Vector(0._f);
    }
};


NAMESPACE_SPH_END
