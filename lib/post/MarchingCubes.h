#pragma once

#include "geometry/Box.h"
#include "objects/containers/Array.h"
#include "objects/wrappers/AutoPtr.h"

NAMESPACE_SPH_BEGIN

class Triangle {
private:
    Vector v[3];

public:
    INLINE Vector& operator[](const Size idx) {
        return v[idx];
    }

    INLINE Vector normal() const {
        ASSERT(this->isValid());
        const Vector v12 = v[2] - v[1];
        const Vector v02 = v[2] - v[0];
        return getNormalized(cross(v12, v02));
    }

    INLINE bool isValid() const {
        const Vector v12 = v[2] - v[1];
        const Vector v02 = v[2] - v[0];
        return sqr(dot(v12, v02)) < (1._f - EPS) * getSqrLength(v12) * getSqrLength(v02);
    }
};


class Cell {
private:
    StaticArray<Vector, 8> points;
    StaticArray<Float, 8> values;

public:
    INLINE Float& value(const Size idx) {
        return values[idx];
    }

    INLINE Vector& node(const Size idx) {
        return points[idx];
    }
};

namespace Abstract {
    class FieldFunction : public Polymorphic {
    public:
        /// Returns the value of the scalar field at given position
        virtual Float operator()(const Vector& pos) = 0;
    };
}

class MarchingCubes {
private:
    /// Input values
    ArrayView<const Vector> r;
    Float surfaceLevel;

    /// Field, isosurface of which we want to triangularize
    AutoPtr<Abstract::FieldFunction> field;

    /// Output array of triangles
    Array<Triangle> triangles;

    /// Cached stuff to avoid re-allocation
    struct {
        Array<Float> phi;
    } cached;

    static Size IDXS1[12];
    static Size IDXS2[12];


public:
    MarchingCubes(ArrayView<const Vector> r,
        const Float surfaceLevel,
        AutoPtr<Abstract::FieldFunction>&& field);

    /// Adds a triangle mesh representing the boundary of particles inside given bounding box into the
    /// internal triangle buffer.
    void addComponent(const Box& box, const Float gridResolution);

    INLINE Array<Triangle>& getTriangles() & {
        return triangles;
    }

    INLINE Array<Triangle> getTriangles() && {
        return std::move(triangles);
    }

private:
    /// Add triangles representing isolevel of the field in given cell into the interal triangle buffer
    INLINE void intersectCell(Cell& cell);

    /// Find the interpolated vertex position based on the surface level
    Vector interpolate(const Vector& v1, const Float p1, const Vector& v2, const Float p2) const;
};


NAMESPACE_SPH_END
