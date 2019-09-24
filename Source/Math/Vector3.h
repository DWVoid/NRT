#pragma once

#include <cmath>
#include <algorithm>
#include <type_traits>

template <class T, class = std::enable_if<std::is_arithmetic_v<T>>>
struct Vec3 {
    union {
        T Data[3];
        struct {
            T X, Y, Z;
        };
    };

    constexpr Vec3() noexcept = default;

    constexpr Vec3(T x, T y, T z) noexcept
            :X(x), Y(y), Z(z) { }

    constexpr explicit Vec3(T v) noexcept
            :X(v), Y(v), Z(v) { }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
    template <typename U, class = std::enable_if_t<std::is_convertible_v<T, U>>>
    constexpr Vec3(const Vec3<U>& r) noexcept
            : X(T(r.X)), Y(T(r.Y)), Z(T(r.Z)) { }
#pragma clang diagnostic pop

    constexpr Vec3 operator+(const Vec3& r) const noexcept { return {X+r.X, Y+r.Y, Z+r.Z}; }

    constexpr Vec3 operator-(const Vec3& r) const noexcept { return {X-r.X, Y-r.Y, Z-r.Z}; }

    template <class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec3 operator*(U r) const noexcept { return {X*r, Y*r, Z*r}; }

    template <class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec3 operator/(U r) const noexcept { return {X/r, Y/r, Z/r}; }

    constexpr Vec3& operator+=(const Vec3& r) noexcept { return (X += r.X, Y += r.Y, Z += r.Z, *this); }

    constexpr Vec3& operator-=(const Vec3& r) noexcept { return (X -= r.X, Y -= r.Y, Z -= r.Z, *this); }

    template <class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec3& operator*=(U r) noexcept { return (X *= r, Y *= r, Z *= r, *this); }

    template <class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec3& operator/=(U r) noexcept { return (X /= r, Y /= r, Z /= r, *this); }

    constexpr Vec3 operator*(const Vec3& r) const noexcept {
        return {Y*r.Z-Z*r.Y, Z*r.X-X*r.Z, X*r.Y-Y*r.X};
    }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3 operator<<(T r) const noexcept { return {X << r, Y << r, Z << r}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3 operator<<(const Vec3& r) const noexcept { return {X << r.X, Y << r.Y, Z << r.Y}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3& operator<<=(T r) noexcept { return (X <<= r, Y <<= r, Z <<= r, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3& operator<<=(const Vec3& r) noexcept { return (X <<= r.X, Y <<= r.Y, Z <<= r.Z, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3 operator>>(T r) const noexcept { return {X >> r, Y >> r, Z >> r}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3 operator>>(const Vec3& r) const noexcept { return {X >> r.X, Y >> r.Y, Z >> r.Z}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3& operator>>=(T r) noexcept { return (X >>= r, Y >>= r, Z >>= r, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3& operator>>=(const Vec3& r) noexcept { return (X >>= r.X, Y >>= r.Y, Z >>= r.Z, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3 operator|(T r) const noexcept { return {X | r, Y | r, Z | r}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3 operator|(const Vec3& r) const noexcept { return {X | r.X, Y | r.Y, Z | r.Z}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3& operator|=(T r) noexcept { return (X |= r, Y |= r, Z |= r, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3& operator|=(const Vec3& r) noexcept { return (X |= r.X, Y |= r.Y, Z |= r.Z, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3 operator&(T r) const noexcept { return {X & r, Y & r, Z & r}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3 operator&(const Vec3& r) const noexcept { return {X & r.X, Y & r.Y, Z & r.Z}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3& operator&=(T r) noexcept { return (X &= r, Y &= r, Z &= r, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3& operator&=(const Vec3& r) noexcept { return (X &= r.X, Y &= r.Y, Z &= r.Z, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3 operator^(T r) const noexcept { return {X ^ r, Y ^ r, Z ^ r}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3 operator^(const Vec3& r) const noexcept { return {X ^ r.X, Y ^ r.Y, Z ^ r.Z}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3& operator^=(T r) noexcept { return (X ^= r, Y ^= r, Z ^= r, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec3& operator^=(const Vec3& r) noexcept { return (X ^= r.X, Y ^= r.Y, Z ^= r.Z, *this); }

    constexpr Vec3 operator-() const noexcept { return {-X, -Y, -Z}; }

    constexpr T LengthSquared() const noexcept { return X*X+Y*Y+Z*Z; }

    constexpr bool operator==(const Vec3& r) const noexcept { return (X==r.X) && (Y==r.Y) && (Z==r.Z); }

    constexpr bool operator<(const Vec3& r) const noexcept { return LengthSquared()<r.LengthSquared(); }

    constexpr bool operator>(const Vec3& r) const noexcept { return LengthSquared()>r.LengthSquared(); }

    constexpr bool operator<=(const Vec3& r) const noexcept { return LengthSquared()<=r.LengthSquared(); }

    constexpr bool operator>=(const Vec3& r) const noexcept { return LengthSquared()>=r.LengthSquared(); }

    constexpr T Dot(const Vec3& r) const noexcept { return X*r.X+Y*r.Y+Z*r.Z; }

    T Length() const noexcept { return std::sqrt(LengthSquared()); }

    void Normalize() noexcept { (*this) /= Length(); }
};

template <class T>
constexpr T Dot(const Vec3<T>& l, const Vec3<T>& r) noexcept { return l.Dot(r); }

template <class T, class U, class = std::enable_if<std::is_arithmetic_v<U>>>
constexpr Vec3<T> operator*(U l, const Vec3<T>& r) noexcept { return r*l; }

template <class T>
double EuclideanDistanceSquared(const Vec3<T>& l, const Vec3<T>& r) noexcept {
    return (l-r).LengthSquared();
}

template <class T>
double EuclideanDistance(const Vec3<T>& l, const Vec3<T>& r) noexcept {
    return (l-r).Length();
}

template <class T>
double DistanceSquared(const Vec3<T>& l, const Vec3<T>& r) noexcept {
    return (l-r).LengthSquared();
}

template <class T>
double Distance(const Vec3<T>& l, const Vec3<T>& r) noexcept {
    return (l-r).Length();
}

template <class T>
constexpr T ChebyshevDistance(const Vec3<T>& l, const Vec3<T>& r) noexcept {
    return std::max(std::max(std::abs(l.X-r.X), std::abs(l.Y-r.Y)), std::abs(l.Z-r.Z));
}

template <class T>
constexpr T ManhattanDistance(const Vec3<T>& l, const Vec3<T>& r) noexcept {
    return std::abs(l.X-r.X)+std::abs(l.Y-r.Y)+std::abs(l.Z-r.Z);
}

using Char3 = Vec3<int8_t>;
using Byte3 = Vec3<uint8_t>;
using Short3 = Vec3<int16_t>;
using UShort3 = Vec3<uint16_t>;
using Int3 = Vec3<int32_t>;
using UInt3 = Vec3<uint32_t>;
using Long3 = Vec3<int64_t>;
using ULong3 = Vec3<uint64_t>;
using Float3 = Vec3<float>;
using Double3 = Vec3<double>;
