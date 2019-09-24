#pragma once

#include <new>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <utility>

class Temp {
public:
    static void* Allocate(const uintptr_t  size) noexcept {
        if (size<=AllocThreshold) {
            return AllocateUnsafe(size);
        }
        else {
            return malloc(size);
        }
    }

    static void Deallocate(void* ptr, uintptr_t  size) noexcept {
        if (size<=AllocThreshold) {
            DeallocateUnsafe(ptr);
        }
        else {
            free(ptr);
        }
    }

    static void Collect() noexcept;

    template <class T, class ...Arg, class = std::enable_if<alignof(T) <= alignof(std::max_align_t)>>
    static T* Allocate(Arg&& ... arg) noexcept {
        if constexpr(sizeof(T) <= AllocThreshold) {
            const auto mem = AllocateUnsafe(sizeof(T));
            return new(mem) T(std::forward<Arg>(arg)...);
        }
        else {
            return new T(std::forward<Arg>(arg)...);
        }
    }

    template <class T>
    static void Deallocate(T* ptr) noexcept {
        if constexpr(sizeof(T) <= AllocThreshold) {
            ptr->~T();
            DeallocateUnsafe(reinterpret_cast<void*>(const_cast<std::remove_cv_t<T>*>(ptr)));
        }
        else {
            delete ptr;
        }
    }
protected:
    static void* AllocateUnsafe(uintptr_t size) noexcept;

    static void DeallocateUnsafe(void* ptr) noexcept {
        static constexpr uintptr_t AlignRev = 0b11'1111'1111'1111'1111'1111;
        static constexpr uintptr_t AlignMask = ~AlignRev;
        reinterpret_cast<std::atomic_uint32_t*>((reinterpret_cast<uint64_t>(ptr) & AlignMask) | Amd64CpuCacheSize)
                ->fetch_add(1);
    }

    static constexpr uintptr_t Amd64CpuCacheSize = 64;
    static constexpr uintptr_t AllocThreshold = 1u << 18u;
};
