#pragma once

#include "Bigint.hpp"

class BigVec3
{
public:
    Bigint x, y, z;

    BigVec3() : x(Bigint()), y(Bigint()), z(Bigint()) {}

    BigVec3(Bigint x_, Bigint y_, Bigint z_)
        : x(x_), y(y_), z(z_) {}

    BigVec3(Bigint x_) : x(x_), y(x_), z(x_) {}

    BigVec3(glm::vec3 vec) : x(Bigint(vec.x)), y(Bigint(vec.y)), z(Bigint(vec.z)) {}

    BigVec3 operator+(const BigVec3 &other) const
    {
        return BigVec3(x + other.x, y + other.y, z + other.z);
    }

    BigVec3 operator-(const BigVec3 &other) const
    {
        return BigVec3(x - other.x, y - other.y, z - other.z);
    }

    BigVec3 operator*(const BigVec3 &other) const
    {
        return BigVec3(x * other.x, y * other.y, z * other.z);
    }

    BigVec3 operator/(const BigVec3 &other) const
    {
        return BigVec3(x / other.x, y / other.y, z / other.z);
    }

    BigVec3 operator+(const Bigint &other) const
    {
        return BigVec3(x + other, y + other, z + other);
    }

    BigVec3 operator-(const Bigint &other) const
    {
        return BigVec3(x - other, y - other, z - other);
    }

    BigVec3 operator*(const Bigint &other) const
    {
        return BigVec3(x * other, y * other, z * other);
    }

    BigVec3 operator/(const Bigint &other) const
    {
        return BigVec3(x / other, y / other, z / other);
    }

    void operator+=(const BigVec3 &other)
    {
        *this = *this + other;
    }

    void operator-=(const BigVec3 &other)
    {
        *this = *this - other;
    }

    void operator*=(const BigVec3 &other)
    {
        *this = *this * other;
    }

    void operator/=(const BigVec3 &other)
    {
        *this = *this / other;
    }

    void operator+=(const Bigint &other)
    {
        *this = *this + other;
    }

    void operator-=(const Bigint &other)
    {
        *this = *this - other;
    }

    void operator*=(const Bigint &other)
    {
        *this = *this * other;
    }

    void operator/=(const Bigint &other)
    {
        *this = *this / other;
    }

    bool isZero() const
    {
        return x.isZero() && y.isZero() && z.isZero();
    }

    Bigint getMaxAbs() const
    {
        const Bigint &a = x.getAbs();
        const Bigint &b = y.getAbs();
        const Bigint &c = z.getAbs();
        return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
    }

    Bigint getMinAbs() const
    {
        const Bigint &a = x.getAbs();
        const Bigint &b = y.getAbs();
        const Bigint &c = z.getAbs();
        return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
    }

    glm::vec3 toFloatVec3() const
    {
        return glm::vec3(
            x.toFloat(),
            y.toFloat(),
            z.toFloat());
    }

    glm::dvec3 toDoubleVec3() const
    {
        return glm::dvec3(
            x.toDouble(),
            y.toDouble(),
            z.toDouble());
    }
};

class BigVec2
{
public:
    Bigint x, y;

    BigVec2() : x(Bigint()), y(Bigint()) {}

    BigVec2(Bigint x, Bigint y)
        : x(x), y(y) {}

    BigVec2(Bigint x_) : x(x_), y(x_) {}

    BigVec2(glm::vec2 vec) : x(Bigint(vec.x)), y(Bigint(vec.y)) {}

    BigVec2 operator+(const BigVec2 &other) const
    {
        return BigVec2(x + other.x, y + other.y);
    }

    BigVec2 operator-(const BigVec2 &other) const
    {
        return BigVec2(x - other.x, y - other.y);
    }

    void operator+=(const BigVec2 &other)
    {
        *this = *this + other;
    }

    void operator-=(const BigVec2 &other)
    {
        *this = *this - other;
    }

    glm::vec2 toFloatVec3() const
    {
        return glm::vec2(
            x.toFloat(),
            y.toFloat());
    }

    glm::dvec2 toDoubleVec3() const
    {
        return glm::dvec2(
            x.toDouble(),
            y.toDouble());
    }
};