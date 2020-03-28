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

    constexpr Vec2() noexcept: Data{} {}

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

    T Length() const noexcept { return T(std::sqrt(LengthSquared())); }

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

using Char2 = Vec2<int8_t>;
using Byte2 = Vec2<uint8_t>;
using Short2 = Vec2<int16_t>;
using UShort2 = Vec2<uint16_t>;
using Int2 = Vec2<int32_t>;
using UInt2 = Vec2<uint32_t>;
using Long2 = Vec2<int64_t>;
using ULong2 = Vec2<uint64_t>;
using Float2 = Vec2<float>;
using Double2 = Vec2<double>;

namespace std {
    template <class T>
    struct hash<Vec2<T>> {
        using argument_type = Vec2<T>;
        using result_type = std::size_t;

        result_type operator()(argument_type const& s) const noexcept {
            const auto floatCnv = [](const double a) noexcept {
                static_assert(std::numeric_limits<double>::is_iec559);
                return *reinterpret_cast<const uint64_t*>(&a);
            };
            const auto hashOne = [floatCnv](const T a) noexcept {
                size_t x;
                if constexpr (std::is_integral_v<T>) x = a; else x = floatCnv(a);
                x = (x) & 0xFFFF00000000FFFFu;
                x = (x | (x << 16u)) & 0x00FF0000FF0000FFu;
                x = (x | (x << 8u)) & 0xF00F00F00F00F00Fu;
                x = (x | (x << 4u)) & 0x30C30C30C30C30C3u;
                x = (x | (x << 2u)) & 0x9249249249249249u;
                return x;
            };
            return hashOne(s.X) | (hashOne(s.Y) << 1u) | (hashOne(s.Z) << 2u);
        }
    };
}
