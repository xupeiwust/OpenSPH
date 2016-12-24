#pragma once

#include "objects/containers/Array.h"
#include "storage/Storage.h"

NAMESPACE_SPH_BEGIN


/// Simple wrapper over array to simplify accumulating values for each interacting pair of particles.
/// This is useful for quantities where the value is determined by direct summation over neighbouring
/// particles rather than by solving evolutionary equation.

template <typename TAccumulate>
class Accumulator : public Noncopyable {
private:
    using Value = typename TAccumulate::Type;

    Array<Value> values;
    TAccumulate functor;

public:
    Accumulator() = default;

    Accumulator(Accumulator&& other)
        : values(std::move(other.values))
        , functor(std::move(other.functor)) {}

    void update(Storage& storage) {
        values.resize(storage.getParticleCnt());
        values.fill(Value(0._f));
        functor.template update(storage);
    }

    /// Accumulate quantity for a pair of particles. This function should only be called once for each pair of
    /// particles; for particles i and j, functor returns Tuple<TValue, TValue> accumulating quantity
    /// increment of i-th and j-th particle.
    INLINE void accumulate(const int i, const int j, const Vector& grad) {
        Value v1, v2;
        tieToTuple(v1, v2) = functor(i, j, grad);
        values[i] += v1;
        values[j] += v2;
    }

    INLINE operator Array<Value>&() { return values; }

    INLINE operator const Array<Value>&() const { return values; }

    INLINE Value& operator[](const int idx) { return values[idx]; }
};

/// Base class for all object owning accumulators. Template parameter pack shall contain types of all
/// accumulators owned by the object.
template <typename... TAccumulate>
class AccumulatorOwner : public Noncopyable {
private:
    Tuple<Accumulator<TAccumulate>&...> accumulators;

public:
    using Accumulating = Tuple<TAccumulate...>;

    /// Constructor taking references to arrays there the accumulated values are stored
    AccumulatorOwner(Accumulator<TAccumulate>&... accumulators)
        : accumulators(accumulators...) {}

    /// Return a reference to array given by accumulator type.
    template <typename TFunctor>
    Array<typename TFunctor::Type>& get() {
        return accumulators.template get<Accumulator<TFunctor>&>();
    }

    INLINE void accumulate(const int i, const int j, const Vector& grad) {
        forEach(accumulators, [i, j, &grad](auto& ac) { ac.accumulate(i, j, grad); });
    }
};

/// \todo better names for accumulator, functors (inside accumulators) and owners
/// Velocity divergence
class DivvImpl {
private:
    ArrayView<const Vector> v;

public:
    using Type = Float;

    void update(Storage& storage) { v = storage.getAll<Vector>(QuantityKey::R)[1]; }

    INLINE Tuple<Float, Float> operator()(const int i, const int j, const Vector& grad) const {
        const Float delta = dot(v[j] - v[i], grad);
        ASSERT(Math::isReal(delta));
        return { delta, delta };
    }
};
using Divv = Accumulator<DivvImpl>;

/// Velocity rotation
class RotvImpl {
private:
    ArrayView<const Vector> v;

public:
    using Type = Vector;

    void update(Storage& storage) { v = storage.getAll<Vector>(QuantityKey::R)[1]; }

    INLINE Tuple<Vector, Vector> operator()(const int i, const int j, const Vector& grad) const {
        const Vector rot = cross(v[j] - v[i], grad);
        return { rot, rot };
    }
};
using Rotv = Accumulator<RotvImpl>;

/// Velocity divergence multiplied by density (right-hand side of continuity equation)
class RhoDivvImpl {
private:
    ArrayView<const Float> m;
    ArrayView<const Vector> v;

public:
    using Type = Float;

    void update(Storage& storage) {
        m = storage.getValue<Float>(QuantityKey::M);
        v = storage.getAll<Vector>(QuantityKey::R)[1];
    }

    INLINE Tuple<Float, Float> operator()(const int i, const int j, const Vector& grad) const {
        const Float delta = dot(v[j] - v[i], grad);
        ASSERT(Math::isReal(delta));
        return { m[j] * delta, m[i] * delta };
    }
};
using RhoDivv = Accumulator<RhoDivvImpl>;

/// Strain rate tensor (symmetrized velocity gradient) multiplied by density
class RhoGradvImpl {
private:
    ArrayView<const Float> m;
    ArrayView<const Vector> v;

public:
    using Type = Tensor;

    void update(Storage& storage) {
        m = storage.getValue<Float>(QuantityKey::M);
        v = storage.getAll<Vector>(QuantityKey::R)[1];
    }

    INLINE Tuple<Tensor, Tensor> operator()(const int i, const int j, const Vector& grad) const {
        const Tensor gradv = outer(v[j] - v[i], grad);
        ASSERT(Math::isReal(gradv));
        return { m[j] * gradv, m[i] * gradv };
    }
};
using RhoGradv = Accumulator<RhoGradvImpl>;

/// Correction tensor ensuring conversion of total angular momentum to the first order.
/// The accumulated value is an inversion of the correction C_ij, applied as multiplicative factor on velocity
/// gradient.
/// \todo They use slightly different SPH equations, check that it's ok
/// \todo It is really symmetric tensor?
class SchaferEtAlCorrectionImpl {
private:
    ArrayView<const Float> m;
    ArrayView<const Float> rho;
    ArrayView<const Vector> r;

public:
    using Type = Tensor;

    void update(Storage& storage) {
        m = storage.getValue<Float>(QuantityKey::M);
        r = storage.getValue<Vector>(QuantityKey::R);
        rho = storage.getValue<Float>(QuantityKey::RHO);
    }

    INLINE Tuple<Tensor, Tensor> operator()(const int i, const int j, const Vector& grad) const {
        Tensor t = outer(r[j] - r[i], grad); // symmetric in i,j ?
        return { m[j] / rho[j] * t, m[i] / rho[i] * t };
    }
};
using SchaferEtAlCorrection = Accumulator<SchaferEtAlCorrectionImpl>;

NAMESPACE_SPH_END
