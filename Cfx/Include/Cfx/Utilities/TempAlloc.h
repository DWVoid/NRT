#pragma once

#include <new>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <memory>
#include "Cfx/Config.h"
#include "System/Memory.h"

class Temp {
public:
    static void* Allocate(const uintptr_t size) noexcept {
        if (size <= AllocThreshold) { return AllocateUnsafe(size); }
        return malloc(size);
    }

    static void Deallocate(void* ptr, const uintptr_t size) noexcept {
        if (size <= AllocThreshold) { DeallocateUnsafe(ptr); }
        else { free(ptr); }
    }

    NRTCORE_API static void Collect() noexcept;

    template <class T, class ...Arg>
    static T* New(Arg&& ... arg) noexcept {
        static_assert(alignof(T) <= alignof(std::max_align_t), "Alignment too large for this allocator");
        if constexpr (sizeof(T) <= AllocThreshold) {
            const auto mem = AllocateUnsafe(sizeof(T));
            return new(mem) T(std::forward<Arg>(arg)...);
        }
        else { return new T(std::forward<Arg>(arg)...); }
    }

    template <class T>
    static void Delete(T* ptr) noexcept {
        if constexpr (sizeof(T) <= AllocThreshold) {
            ptr->~T();
            DeallocateUnsafe(reinterpret_cast<void*>(const_cast<std::remove_cv_t<T>*>(ptr)));
        }
        else { delete ptr; }
    }

    template <class T>
    struct Allocator {
    private:
        static constexpr uintptr_t Align() noexcept {
            const auto trim = sizeof(T) / alignof(T) * alignof(T);
            return trim != sizeof(T) ? trim + alignof(T) : sizeof(T);
        }

        // Helper Const
        static constexpr uintptr_t Alignment = alignof(T);
        static constexpr uintptr_t AlignedSize = Align();
        std::allocator<T> mInner {};
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using is_always_equal = std::true_type;
        using propagate_on_container_move_assignment = std::true_type;

        constexpr Allocator() noexcept = default;

        template <class U>
        explicit constexpr Allocator(const Allocator<U>&) noexcept {}

        T* address(T& __x) const noexcept {return std::addressof(__x);}

        const T* address(const T& __x) const noexcept {return std::addressof(__x);}

        [[nodiscard]] T* allocate(const std::size_t n) {
            if constexpr (Alignment <= alignof(std::max_align_t)) {
                const auto size = n > 1 ? AlignedSize * n : sizeof(T);
                if (size <= AllocThreshold) { return reinterpret_cast<T*>(AllocateUnsafe(size)); }
            }
            return mInner.allocate(n);
        }

        void deallocate(T* p, const std::size_t n) noexcept {
            if constexpr (Alignment <= alignof(std::max_align_t)) {
                if ((n > 1 ? AlignedSize * n : sizeof(T)) <= AllocThreshold) {
                    DeallocateUnsafe(reinterpret_cast<void*>(const_cast<std::remove_cv_t<T>*>(p)));
                    return;
                }
            }
            mInner.deallocate(p, n);
        }
    };

protected:
    NRTCORE_API static void* AllocateUnsafe(uintptr_t size) noexcept;

    static void DeallocateUnsafe(void* ptr) noexcept {
        static constexpr uintptr_t AlignRev = 0b11'1111'1111'1111'1111'1111;
        static constexpr uintptr_t AlignMask = ~AlignRev;
        reinterpret_cast<std::atomic_uint32_t*>((reinterpret_cast<uint64_t>(ptr) & AlignMask) | Amd64CpuCacheSize)
            ->fetch_add(1);
    }

    static constexpr uintptr_t Amd64CpuCacheSize = NEWorld::System::Memory::Amd64CpuCacheSize;
    static constexpr uintptr_t AllocThreshold = 1u << 18u;
};
