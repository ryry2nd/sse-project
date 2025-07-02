#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Bigint.hpp"

class BigVec3
{
public:
    Bigint x, y, z;

    BigVec3(const BigVec3 &other) noexcept
        : x(other.x), y(other.y), z(other.z)
    {
    }

    BigVec3(BigVec3 &&other) noexcept
        : x(std::move(other.x)), y(std::move(other.y)), z(std::move(other.z)) {}

    // Copy assignment
    BigVec3 &operator=(const BigVec3 &other) noexcept
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    // Move assignment
    BigVec3 &operator=(BigVec3 &&other) noexcept
    {
        x = std::move(other.x);
        y = std::move(other.y);
        z = std::move(other.z);
        return *this;
    }

    BigVec3()
    {
    }

    BigVec3(Bigint x_, Bigint y_, Bigint z_)
        : x(x_), y(y_), z(z_) {}

    BigVec3(Bigint x_) : x(x_), y(x_), z(x_) {}

    BigVec3(glm::vec3 vec) : x(Bigint(vec.x)), y(Bigint(vec.y)), z(Bigint(vec.z)) {}

    inline BigVec3 operator+(const BigVec3 &other) const noexcept
    {
        BigVec3 result;
        result.x = std::move(x + other.x);
        result.y = std::move(y + other.y);
        result.z = std::move(z + other.z);
        return result;
    }

    inline BigVec3 operator-(const BigVec3 &other) const noexcept
    {
        BigVec3 result;
        result.x = std::move(x - other.x);
        result.y = std::move(y - other.y);
        result.z = std::move(z - other.z);
        return result;
    }

    inline BigVec3 operator*(const BigVec3 &other) const noexcept
    {
        BigVec3 result;
        result.x = std::move(x * other.x);
        result.y = std::move(y * other.y);
        result.z = std::move(z * other.z);
        return result;
    }

    inline BigVec3 operator/(const BigVec3 &other) const
    {
        BigVec3 result;
        result.x = std::move(x / other.x);
        result.y = std::move(y / other.y);
        result.z = std::move(z / other.z);
        return result;
    }

    inline BigVec3 operator+(const Bigint &other) const noexcept
    {
        BigVec3 result;
        result.x = std::move(x + other);
        result.y = std::move(y + other);
        result.z = std::move(z + other);
        return result;
    }

    inline BigVec3 operator-(const Bigint &other) const noexcept
    {
        BigVec3 result;
        result.x = std::move(x - other);
        result.y = std::move(y - other);
        result.z = std::move(z - other);
        return result;
    }

    inline BigVec3 operator*(const Bigint &other) const noexcept
    {
        BigVec3 result;
        result.x = std::move(x * other);
        result.y = std::move(y * other);
        result.z = std::move(z * other);
        return result;
    }

    inline BigVec3 operator/(const Bigint &other) const
    {
        BigVec3 result;
        result.x = std::move(x / other);
        result.y = std::move(y / other);
        result.z = std::move(z / other);
        return result;
    }

    void operator+=(const BigVec3 &other) noexcept
    {
        *this = *this + other;
    }

    void operator-=(const BigVec3 &other) noexcept
    {
        *this = *this - other;
    }

    void operator*=(const BigVec3 &other) noexcept
    {
        *this = *this * other;
    }

    void operator/=(const BigVec3 &other)
    {
        *this = *this / other;
    }

    void operator+=(const Bigint &other) noexcept
    {
        *this = *this + other;
    }

    void operator-=(const Bigint &other) noexcept
    {
        *this = *this - other;
    }

    void operator*=(const Bigint &other) noexcept
    {
        *this = *this * other;
    }

    void operator/=(const Bigint &other)
    {
        *this = *this / other;
    }

    inline bool isZero() const noexcept
    {
        return x.isZero() && y.isZero() && z.isZero();
    }

    inline Bigint getMaxAbs() const noexcept
    {
        const Bigint &a = x.getAbs();
        const Bigint &b = y.getAbs();
        const Bigint &c = z.getAbs();
        return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
    }

    inline Bigint getMinAbs() const noexcept
    {
        const Bigint &a = x.getAbs();
        const Bigint &b = y.getAbs();
        const Bigint &c = z.getAbs();
        return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
    }

    inline glm::vec3 toFloatVec3() const noexcept
    {
        return glm::vec3(
            x.toFloat(),
            y.toFloat(),
            z.toFloat());
    }

    inline glm::dvec3 toDoubleVec3() const noexcept
    {
        return glm::dvec3(
            x.toDouble(),
            y.toDouble(),
            z.toDouble());
    }
};