#pragma once

#include "common/ForwardDecl.h"
#include "objects/containers/Array.h"
#include "objects/wrappers/Value.h"
#include "quantities/QuantityIds.h"

NAMESPACE_SPH_BEGIN

enum class CriterionId {
    INITIAL_VALUE, ///< Timestep is not computed, using given initial value
    MAXIMAL_VALUE, ///< Timestep given by selected maximal value
    DERIVATIVE,    ///< Timestep based on value-to-derivative ratio
    CFL_CONDITION, ///< Timestep computed using CFL condition
    ACCELERATION   ///< Timestep constrained by acceleration condition
};

std::ostream& operator<<(std::ostream& stream, const CriterionId id);

namespace Abstract {

    /// Base class for timestep setters.
    class TimeStepCriterion : public Polymorphic {
    public:
        /// Returns the current time step.
        /// \param storage Storage containing all physical quantities from which the time step is determined.
        /// \param maxStep Maximal allowed time step.
        /// \param stats Optional parameter used to save statistics of the criterion.
        /// \returns Tuple, containing computed time step and ID of quantity that determined the value.
        virtual Tuple<Float, CriterionId> compute(Storage& storage,
            const Float maxStep,
            Optional<Statistics&> stats = NOTHING) = 0;
    };
}

/// Criterion setting time step based on value-to-derivative ratio for time-dependent quantities.
/// \todo add variability, currently sets timestep by mininum of all quantities and all particles, that may be
/// too strict and limiting (one outlier will set timestep for all).
/// \todo currently only used on first-order quantities.
class DerivativeCriterion : public Abstract::TimeStepCriterion {
private:
    Float factor;

public:
    DerivativeCriterion(const RunSettings& settings);

    virtual Tuple<Float, CriterionId> compute(Storage& storage,
        const Float maxStep,
        Optional<Statistics&> stats = NOTHING) override;
};

/// Criterion settings time step based on computed acceleration of particles.
class AccelerationCriterion : public Abstract::TimeStepCriterion {
public:
    virtual Tuple<Float, CriterionId> compute(Storage& storage,
        const Float maxStep,
        Optional<Statistics&> stats = NOTHING) override;
};

/// Time step based on CFL criterion.
class CourantCriterion : public Abstract::TimeStepCriterion {
private:
    Float courant;

public:
    CourantCriterion(const RunSettings& settings);

    /// Storage must contain at least positions of particles and sound speed, checked by assert.
    virtual Tuple<Float, CriterionId> compute(Storage& storage,
        const Float maxStep,
        Optional<Statistics&> stats = NOTHING) override;
};


/// Helper "virtual" criterion, wrapping multiple criteria under Abstract::AdaptiveTimeStep interface.
class MultiCriterion : public Abstract::TimeStepCriterion {
private:
    StaticArray<std::unique_ptr<Abstract::TimeStepCriterion>, 3> criteria;

public:
    MultiCriterion(const RunSettings& settings);

    virtual Tuple<Float, CriterionId> compute(Storage& storage,
        const Float maxStep,
        Optional<Statistics&> stats = NOTHING) override;
};

NAMESPACE_SPH_END
