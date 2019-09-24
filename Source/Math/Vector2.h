#pragma once

#include <cmath>
#include <algorithm>
#include <type_traits>

template <class T, class = std::enable_if<std::is_arithmetic_v<T>>>
struct Vec2 {
    union {
        T Data[2];
        struct {
            T X, Y;
        };
    };

    constexpr Vec2() noexcept = default;

    constexpr Vec2(T x, T y) noexcept
            :X(x), Y(y) { }

    constexpr explicit Vec2(T v) noexcept
            :X(v), Y(v) { }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
    template <typename U, class = std::enable_if_t<std::is_convertible_v<T, U>>>
    constexpr Vec2(const Vec2<U>& r) noexcept
            : X(T(r.X)), Y(T(r.Y)) { }
#pragma clang diagnostic pop

    constexpr Vec2 operator+(const Vec2& r) const noexcept { return {X+r.X, Y+r.Y}; }

    constexpr Vec2 operator-(const Vec2& r) const noexcept { return {X-r.X, Y-r.Y}; }

    template <class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec2 operator*(U r) const noexcept { return {X*r, Y*r}; }

    template <class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec2 operator/(U r) const noexcept { return {X/r, Y/r}; }

    constexpr Vec2& operator+=(const Vec2& r) noexcept { return (X += r.X, Y += r.Y, *this); }

    constexpr Vec2& operator-=(const Vec2& r) noexcept { return (X -= r.X, Y -= r.Y, *this); }

    template <class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec2& operator*=(U r) noexcept { return (X *= r, Y *= r, *this); }

    template <class U, class = std::enable_if<std::is_arithmetic_v<U>>>
    constexpr Vec2& operator/=(U r) noexcept { return (X /= r, Y /= r, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2 operator<<(T r) const noexcept { return {X << r, Y << r}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2 operator<<(const Vec2& r) const noexcept { return {X << r.X, Y << r.Y}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2& operator<<=(T r) noexcept { return (X <<= r, Y <<= r, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2& operator<<=(const Vec2& r) noexcept { return (X <<= r.X, Y <<= r.Y, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2 operator>>(T r) const noexcept { return {X >> r, Y >> r}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2 operator>>(const Vec2& r) const noexcept { return {X >> r.X, Y >> r.Y}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2& operator>>=(T r) noexcept { return (X >>= r, Y >>= r, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2& operator>>=(const Vec2& r) noexcept { return (X >>= r.X, Y >>= r.Y, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2 operator|(T r) const noexcept { return {X | r, Y | r}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2 operator|(const Vec2& r) const noexcept { return {X | r.X, Y | r.Y}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2& operator|=(T r) noexcept { return (X |= r, Y |= r, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2& operator|=(const Vec2& r) noexcept { return (X |= r.X, Y |= r.Y, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2 operator&(T r) const noexcept { return {X & r, Y & r}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2 operator&(const Vec2& r) const noexcept { return {X & r.X, Y & r.Y,}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2& operator&=(T r) noexcept { return (X &= r, Y &= r, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2& operator&=(const Vec2& r) noexcept { return (X &= r.X, Y &= r.Y, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2 operator^(T r) const noexcept { return {X ^ r, Y ^ r}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2 operator^(const Vec2& r) const noexcept { return {X ^ r.X, Y ^ r.Y}; }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2& operator^=(T r) noexcept { return (X ^= r, Y ^= r, *this); }

    template <class = std::enable_if<std::is_integral_v<T>>>
    constexpr Vec2& operator^=(const Vec2& r) noexcept { return (X ^= r.X, Y ^= r.Y, *this); }

    constexpr Vec2 operator-() const noexcept { return {-X, -Y}; }

    constexpr T LengthSquared() const noexcept { return X*X+Y*Y; }

    constexpr bool operator==(const Vec2& r) const noexcept { return (X==r.X) && (Y==r.Y); }

    constexpr bool operator<(const Vec2& r) const noexcept { return LengthSquared()<r.LengthSquared(); }

    constexpr bool operator>(const Vec2& r) const noexcept { return LengthSquared()>r.LengthSquared(); }

    constexpr bool operator<=(const Vec2& r) const noexcept { return LengthSquared()<=r.LengthSquared(); }

    constexpr bool operator>=(const Vec2& r) const noexcept { return LengthSquared()>=r.LengthSquared(); }

    constexpr T Dot(const Vec2& r) const noexcept { return X*r.X+Y*r.Y; }

    T Length() const noexcept { return std::sqrt(LengthSquared()); }

    void Normalize() noexcept { (*this) /= Length(); }
};

template <class T>
constexpr T Dot(const Vec2<T>& l, const Vec2<T>& r) noexcept { return l.Dot(r); }

template <class T, class U, class = std::enable_if<std::is_arithmetic_v<U>>>
constexpr Vec2<T> operator*(U l, const Vec2<T>& r) noexcept { return r*l; }

template <class T>
double EuclideanDistanceSquared(const Vec2<T>& l, const Vec2<T>& r) noexcept {
    return (l-r).LengthSquared();
}

template <class T>
double EuclideanDistance(const Vec2<T>& l, const Vec2<T>& r) noexcept {
    return (l-r).Length();
}

template <class T>
double DistanceSquared(const Vec2<T>& l, const Vec2<T>& r) noexcept {
    return (l-r).LengthSquared();
}

template <class T>
double Distance(const Vec2<T>& l, const Vec2<T>& r) noexcept {
    return (l-r).Length();
}

template <class T>
constexpr T ChebyshevDistance(const Vec2<T>& l, const Vec2<T>& r) noexcept {
    return std::max(std::max(std::abs(l.X-r.X), std::abs(l.Y-r.Y)), std::abs(l.Z-r.Z));
}

template <class T>
constexpr T ManhattanDistance(const Vec2<T>& l, const Vec2<T>& r) noexcept {
    return std::abs(l.X-r.X)+std::abs(l.Y-r.Y)+std::abs(l.Z-r.Z);
}

using Char3 = Vec2<int8_t>;
using Byte3 = Vec2<uint8_t>;
using Short3 = Vec2<int16_t>;
using UShort3 = Vec2<uint16_t>;
using Int3 = Vec2<int32_t>;
using UInt3 = Vec2<uint32_t>;
using Long3 = Vec2<int64_t>;
using ULong3 = Vec2<uint64_t>;
using Float3 = Vec2<float>;
using Double3 = Vec2<double>;
