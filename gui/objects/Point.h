#pragma once

/// \file Point.h
/// \brief Simple 2D vector with integer coordinates. Provides conversion from and to wxPoint.
/// \author Pavel Sevecek (sevecek at sirrah.troja.mff.cuni.cz)
/// \date 2016-2021

#include "math/MathUtils.h"
#include "objects/Object.h"
#include "objects/containers/String.h"
#include "objects/utility/IteratorAdapters.h"
#include "objects/wrappers/Optional.h"
#include <wx/gdicmn.h>

NAMESPACE_SPH_BEGIN

template <typename T, typename TDerived>
struct BasicPoint {
    T x, y;

    BasicPoint() = default;

    BasicPoint(const T x, const T y)
        : x(x)
        , y(y) {}

    T& operator[](const Size index) {
        SPH_ASSERT(index < 2);
        return reinterpret_cast<T*>(this)[index];
    }

    const T& operator[](const Size index) const {
        SPH_ASSERT(index < 2);
        return reinterpret_cast<const T*>(this)[index];
    }

    TDerived& operator+=(const TDerived& other) {
        x += other.x;
        y += other.y;
        return static_cast<TDerived&>(*this);
    }

    TDerived& operator-=(const TDerived& other) {
        x -= other.x;
        y -= other.y;
        return static_cast<TDerived&>(*this);
    }

    TDerived& operator*=(const float factor) {
        x = T(x * factor);
        y = T(y * factor);
        return static_cast<TDerived&>(*this);
    }

    TDerived& operator/=(const float factor) {
        x = T(x / factor);
        y = T(y / factor);
        return static_cast<TDerived&>(*this);
    }

    TDerived operator+(const TDerived& other) const {
        TDerived result(static_cast<const TDerived&>(*this));
        result += other;
        return result;
    }

    TDerived operator-(const TDerived& other) const {
        TDerived result(static_cast<const TDerived&>(*this));
        result -= other;
        return result;
    }

    TDerived operator*(const float factor) const {
        TDerived result(static_cast<const TDerived&>(*this));
        result *= factor;
        return result;
    }

    TDerived operator/(const float factor) const {
        TDerived result(static_cast<const TDerived&>(*this));
        result /= factor;
        return result;
    }

    bool operator==(const TDerived& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const TDerived& other) const {
        return !(*this == other);
    }

    template <typename TStream>
    friend TStream& operator<<(TStream& stream, const BasicPoint& p) {
        stream << p.x << " " << p.y;
        return stream;
    }

    template <typename TStream>
    friend TStream& operator>>(TStream& stream, BasicPoint& p) {
        stream >> p.x >> p.y;
        return stream;
    }
};

struct Pixel : public BasicPoint<int, Pixel> {
    Pixel() = default;

    Pixel(const int x, const int y)
        : BasicPoint<int, Pixel>(x, y) {}

    explicit Pixel(const wxPoint point)
        : BasicPoint<int, Pixel>(point.x, point.y) {}

    explicit operator wxPoint() const {
        return wxPoint(this->x, this->y);
    }
};

struct Coords : public BasicPoint<float, Coords> {
    Coords() = default;

    Coords(const float x, const float y)
        : BasicPoint<float, Coords>(x, y) {}

    explicit Coords(const Pixel p)
        : BasicPoint<float, Coords>(p.x, p.y) {}

    using BasicPoint<float, Coords>::operator*;
    using BasicPoint<float, Coords>::operator/;

    Coords operator*(const Coords& other) const {
        Coords res = *this;
        res.x *= other.x;
        res.y *= other.y;
        return res;
    }

    Coords operator/(const Coords& other) const {
        Coords res = *this;
        SPH_ASSERT(other.x != 0.f && other.y != 0.f);
        res.x /= other.x;
        res.y /= other.y;
        return res;
    }

    explicit operator Pixel() const {
        return Pixel(int(x), int(y));
    }

    explicit operator wxPoint() const {
        return wxPoint(int(x), int(y));
    }
};

template <>
INLINE Optional<Pixel> fromString(const String& s) {
    std::wstringstream ss(s.toUnicode());
    Pixel p;
    ss >> p;
    if (ss) {
        return p;
    } else {
        return NOTHING;
    }
}

template <typename T, typename TDerived>
INLINE float getLength(const BasicPoint<T, TDerived>& p) {
    return sqrt(float(sqr(p.x) + sqr(p.y)));
}

class Rectangle {
private:
    Pixel minBound;
    Pixel maxBound;

public:
    Rectangle() = default;

    Rectangle(const Pixel& lower, const Pixel& upper)
        : minBound(lower)
        , maxBound(upper) {}

    static Rectangle window(const Pixel center, const Size radius) {
        return Rectangle(center - Pixel(radius, radius), center + Pixel(radius, radius));
    }

    Pixel lower() const {
        return minBound;
    }

    Pixel upper() const {
        return maxBound;
    }

    Pixel size() const {
        return maxBound - minBound;
    }

    bool empty() const {
        return maxBound.x < minBound.x || maxBound.y < minBound.y;
    }

    bool contains(const Pixel& p) const {
        return p.x >= minBound.x && p.y >= minBound.y && p.x <= maxBound.x && p.y <= maxBound.y;
    }

    Rectangle intersect(const Rectangle& other) const {
        Rectangle is;
        is.minBound.x = max(minBound.x, other.minBound.x);
        is.minBound.y = max(minBound.y, other.minBound.y);
        is.maxBound.x = min(maxBound.x, other.maxBound.x);
        is.maxBound.y = min(maxBound.y, other.maxBound.y);
        SPH_ASSERT(!is.empty());
        return is;
    }

    IndexSequence colRange() const {
        return IndexSequence(minBound.x, maxBound.x);
    }

    IndexSequence rowRange() const {
        return IndexSequence(minBound.y, maxBound.y);
    }
};

NAMESPACE_SPH_END
