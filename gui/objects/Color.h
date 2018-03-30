#pragma once

#include "objects/geometry/Vector.h"
#include <wx/colour.h>

NAMESPACE_SPH_BEGIN

class Color {
private:
    BasicVector<float> data;

    Color(const BasicVector<float>& data)
        : data(data) {}

public:
    Color() = default;

    Color(const float r, const float g, const float b)
        : data(r, g, b) {}

    Color(const float gray)
        : data(gray) {}

    Color(const Color& other)
        : data(other.data) {}

    explicit Color(const wxColour& other)
        : data(other.Red() / 255.f, other.Green() / 255.f, other.Blue() / 255.f) {}

    explicit operator wxColour() const {
        return wxColour(int(data[0] * 255.f), int(data[1] * 255.f), int(data[2] * 255.f));
    }

    float operator[](const Size idx) const {
        return data[idx];
    }

    float& operator[](const Size idx) {
        return data[idx];
    }

    Color operator*(const float value) const {
        return data * value;
    }

    Color operator/(const float value) const {
        return data / value;
    }

    Color operator*(const Color& other) const {
        return data * other.data;
    }

    Color operator+(const Color& other) const {
        return data + other.data;
    }

    Color& operator+=(const Color& other) {
        *this = *this + other;
        return *this;
    }

    /// Returns a color darker by given factor (in interval [0, 1], where 0 = current color, 1 = black)
    Color darken(const float amount) const {
        ASSERT(amount >= 0.f && amount <= 1.f);
        return (1.f - amount) * data;
    }

    /// Returns a color brighter by given factor (in interval [0, INFTY], where 0 = current color, 1 = 100%
    /// more brighter, etc.)
    Color brighten(const float amount) const {
        ASSERT(amount >= 0.f);
        return (1._f + amount) * data;
    }

    static Color red() {
        return Color(1.f, 0.f, 0.f);
    }

    static Color green() {
        return Color(0.f, 1.f, 0.f);
    }

    static Color blue() {
        return Color(0.f, 0.f, 1.f);
    }

    static Color black() {
        return Color(0.f, 0.f, 0.f);
    }

    static Color white() {
        return Color(1.f, 1.f, 1.f);
    }

    static Color gray(const float value = 0.5f) {
        return Color(value, value, value);
    }
};

NAMESPACE_SPH_END
