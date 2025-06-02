#pragma once

#include <vector>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

#include <boost/multiprecision/cpp_int.hpp>

class Bigint
{
public:
    boost::multiprecision::cpp_int MAX_DOUBLE = boost::multiprecision::cpp_int(std::numeric_limits<double>::max());
    boost::multiprecision::cpp_int MAX_FLOAT = boost::multiprecision::cpp_int(std::numeric_limits<float>::max());

    static constexpr unsigned long long DIGITS = 5;
    static constexpr unsigned long long SCALE = 100000;
    boost::multiprecision::cpp_int value;

    Bigint()
    {
        value = boost::multiprecision::cpp_int();
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

    Bigint operator+(const Bigint &other) const
    {
        Bigint result;
        result.value = value + other.value;
        return result;
    }

    Bigint operator-(const Bigint &other) const
    {
        Bigint result;
        result.value = value - other.value;
        return result;
    }

    Bigint operator*(const Bigint &other) const
    {
        Bigint result;
        result.value = (value * other.value) / SCALE;
        return result;
    }

    Bigint operator/(const Bigint &other) const
    {
        Bigint result;
        result.value = (value * SCALE) / other.value;
        return result;
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
        return value.is_zero();
    }

    bool operator<(const Bigint &other) const
    {
        return value < other.value;
    }

    bool operator>(const Bigint &other) const
    {
        return value > other.value;
    }

    bool operator==(const Bigint &other) const
    {
        return value == other.value;
    }

    bool operator!=(const Bigint &other) const
    {
        return value != other.value;
    }

    bool operator<=(const Bigint &other) const
    {
        return value <= other.value;
    }

    bool operator>=(const Bigint &other) const
    {
        return value >= other.value;
    }

    std::string toString() const
    {
        boost::multiprecision::cpp_int intPart = value / SCALE;
        boost::multiprecision::cpp_int fracPart = abs(value % SCALE);

        std::ostringstream out;
        out << intPart << "." << std::setw(DIGITS) << std::setfill('0') << fracPart;
        return out.str();
    }

    Bigint getAbs() const
    {
        Bigint ret;
        ret.value = boost::multiprecision::abs(this->value);
        return ret;
    }

    float toFloat() const
    {
        if (value > MAX_FLOAT)
            return INFINITY;
        return static_cast<float>(value) / SCALE;
    }

    double toDouble() const
    {
        if (value > MAX_DOUBLE)
            return INFINITY;
        return static_cast<double>(value) / SCALE;
    }

    Bigint sqrt() const
    {
        if (value < 0)
            throw std::domain_error("Square root of negative number");

        if (value == 0)
            return Bigint(0);

        boost::multiprecision::cpp_int x = value;
        boost::multiprecision::cpp_int guess = x / SCALE;

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
};