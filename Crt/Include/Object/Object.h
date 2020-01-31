#pragma once

#include <Compile/Config.h>

namespace NEWorld {
    template <class T = void> struct IEquitable;

    template <>
    struct IEquitable<void> {
        [[nodiscard]] virtual bool Equals(const IEquitable& rhs) const noexcept = 0;
        bool operator == (const IEquitable& rhs) const noexcept { return Equals(rhs); }
    };

    template <class T>
    struct IEquitable: virtual IEquitable<void> {
        [[nodiscard]] virtual bool Equals(const T& rhs) const noexcept = 0;
        bool operator == (const T& rhs) const noexcept { return Equals(rhs); }
    };

    template <class T = void> struct IComparable;

    template <>
    struct IComparable<void> : virtual IEquitable<void> {
        [[nodiscard]] bool Equals(const IEquitable& rhs) const noexcept override {
            return Compares(dynamic_cast<const IComparable&>(rhs)) == 0; // NOLINT
        }
        [[nodiscard]] virtual int Compares(const IComparable& rhs) const noexcept = 0;
        bool operator < (const IComparable& rhs) const noexcept { return Compares(rhs) < 0; }
        bool operator > (const IComparable& rhs) const noexcept { return Compares(rhs) > 0; }
        bool operator <= (const IComparable& rhs) const noexcept { return Compares(rhs) <= 0; }
        bool operator >= (const IComparable& rhs) const noexcept { return Compares(rhs) >= 0; }
    };

    template <class T>
    struct IComparable: virtual IComparable<void>, virtual IEquitable<T> {
        [[nodiscard]] bool Equals(const T& rhs) const noexcept override { return Compares(rhs) == 0; }
        [[nodiscard]] virtual int Compares(const T& rhs) const noexcept { return IComparable<void>::Compares(rhs); }
        bool operator < (const T& rhs) const noexcept { return Compares(rhs) < 0; }
        bool operator > (const T& rhs) const noexcept { return Compares(rhs) > 0; }
        bool operator <= (const T& rhs) const noexcept { return Compares(rhs) <= 0; }
        bool operator >= (const T& rhs) const noexcept { return Compares(rhs) >= 0; }
    };

    template <class T>
    struct IIterator : virtual IEquitable<IIterator<T>> {
        virtual ~IIterator() noexcept = default;
        virtual T& Current() noexcept = 0;
        virtual bool MoveNext() = 0;
    };
}