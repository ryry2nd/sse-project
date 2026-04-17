#pragma once

#include <cmath>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include "spdlog/spdlog.h"

#define DIGITS 5
#define SCALE 100000

struct Bigint
{
    static inline const boost::multiprecision::cpp_int MAX_DOUBLE = boost::multiprecision::cpp_int(std::numeric_limits<double>::max());
    static inline const boost::multiprecision::cpp_int MAX_FLOAT = boost::multiprecision::cpp_int(std::numeric_limits<float>::max());

    boost::multiprecision::cpp_int value;

    Bigint(const Bigint &other) noexcept
        : value(other.value)
    {
    }

    Bigint(Bigint &&other) noexcept : value(std::move(other.value))
    {
    }

    Bigint &operator=(const Bigint &other) noexcept
    {
        if (this != &other)
        {
            value = other.value;
        }
        return *this;
    }

    Bigint &operator=(Bigint &&other) noexcept
    {
        if (this != &other)
        {
            value = std::move(other.value);
        }
        return *this;
    }

    Bigint()
    {
    }

    Bigint(double d)
    {
        double fd = floor(d);
        value = boost::multiprecision::cpp_int(fd) * SCALE;
        value += boost::multiprecision::cpp_int((d - fd) * SCALE);
    }

    Bigint(float f)
    {
        float fd = floor(f);
        value = boost::multiprecision::cpp_int(fd) * SCALE;
        value += boost::multiprecision::cpp_int((f - fd) * SCALE);
    }

    Bigint(int i)
    {
        value = boost::multiprecision::cpp_int(i) * SCALE;
    }

    Bigint(long l)
    {
        value = boost::multiprecision::cpp_int(l) * SCALE;
    }

    Bigint(long long l)
    {
        value = boost::multiprecision::cpp_int(l) * SCALE;
    }

    Bigint(const std::string &s)
    {
        size_t dot = s.find('.');
        std::string intPart = (dot == std::string::npos) ? s : s.substr(0, dot);
        std::string fracPart = (dot == std::string::npos) ? "" : s.substr(dot + 1);

        while (fracPart.length() < DIGITS)
            fracPart += '0';
        if (fracPart.length() > DIGITS)
            fracPart = fracPart.substr(0, DIGITS);

        value = boost::multiprecision::cpp_int(intPart + fracPart);
    }

    inline Bigint operator+(const Bigint &other) const noexcept
    {
        Bigint result;
        result.value = value + other.value;
        return result;
    }

    inline Bigint operator-(const Bigint &other) const noexcept
    {
        Bigint result;
        result.value = value - other.value;
        return result;
    }

    inline Bigint operator*(const Bigint &other) const noexcept
    {
        Bigint result;
        result.value = (value * other.value) / SCALE;
        return result;
    }

    inline Bigint operator/(const Bigint &other) const
    {
        if (other == 0) {spdlog::error("divide by zero error"); return Bigint();};
        Bigint result;
        result.value = (value * SCALE) / other.value;
        return result;
    }

    inline Bigint operator<<(const size_t &other) const
    {
        Bigint result;
        result.value = value << other;
        return result;
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

    void operator/=(const Bigint &other) noexcept
    {
        if (other == 0) {spdlog::error("divide by zero error"); return;};
        
        *this = *this / other;
    }

    inline bool isZero() const noexcept
    {
        return value.is_zero();
    }

    inline bool operator<(const Bigint &other) const noexcept
    {
        return value < other.value;
    }

    inline bool operator>(const Bigint &other) const noexcept
    {
        return value > other.value;
    }

    inline bool operator==(const Bigint &other) const noexcept
    {
        return value == other.value;
    }

    inline bool operator!=(const Bigint &other) const noexcept
    {
        return value != other.value;
    }

    inline bool operator<=(const Bigint &other) const noexcept
    {
        return value <= other.value;
    }

    inline bool operator>=(const Bigint &other) const noexcept
    {
        return value >= other.value;
    }

    std::string toString() const
    {
        using boost::multiprecision::cpp_int;

        cpp_int intPart = value / SCALE;
        cpp_int fracPart = abs(value % SCALE);

        std::ostringstream out;
        out << intPart << "." << std::setw(DIGITS) << std::setfill('0') << fracPart;
        std::string result = out.str();

        // Remove trailing zeros from the fractional part
        while (!result.empty() && result.back() == '0')
            result.pop_back();

        // If decimal point is the last character, remove it too
        if (!result.empty() && result.back() == '.')
            result.pop_back();

        return result;
    }

    inline Bigint getAbs() const noexcept
    {
        Bigint ret;
        ret.value = boost::multiprecision::abs(this->value);
        return ret;
    }

    inline float toFloat() const noexcept
    {
        if (value > MAX_FLOAT)
            return INFINITY;
        return static_cast<float>(value) / SCALE;
    }

    inline double toDouble() const noexcept
    {
        if (value > MAX_DOUBLE)
            return INFINITY;
        return static_cast<double>(value) / SCALE;
    }

    Bigint sqrt() const
    {
        if (value < 0) {
            spdlog::error("Square root of negative number");
            return Bigint(0);
        }

        if (value == 0)
            return Bigint(0);

        boost::multiprecision::cpp_int x = value;
        boost::multiprecision::cpp_int guess = x / SCALE;
        if (guess == 0)
            guess = 1;

        // Newton-Raphson iterations
        for (int i = 0; i < 100; ++i)
        {
            boost::multiprecision::cpp_int nextGuess = (guess + (x * SCALE) / guess) / 2;
            if (abs(nextGuess - guess) <= 1)
                break;
            guess = nextGuess;
        }

        Bigint result;
        result.value = guess;
        return result;
    }

    // uuhhh, this is for fun, in case i want to make things a googl meters apart, put whatever number here, see what happens, its pritty cool
    static Bigint getHoweverManyDigits(size_t numZeros)
    {
        if (numZeros == 0)
        {
            return Bigint();
        }

        return Bigint(1) << numZeros;
    };
};

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