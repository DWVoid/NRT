#pragma once

#include "Meta.h"

namespace NEWorld {
    class ServiceInfo : public ClassAttribute {
    public:
        constexpr ServiceInfo(const char* name, const double priority, void* mem) noexcept
                :Mem(mem), Name(name), Priority(priority) { }
        NRTCRT_API ~ServiceInfo() noexcept;
        NRTCRT_API void OnRuntimeAttach() noexcept override;
        void* const Mem;
        const char* const Name;
        const double Priority = 0.0;
    };

    class ServiceHandle {
    public:
        NRTCRT_API explicit ServiceHandle(const char* name);
        NRTCRT_API ~ServiceHandle();
        [[nodiscard]] operator bool() const noexcept { return mObj.first; } // NOLINT
        template <class T> T& Get() noexcept { return mObj.first->As<T>(); }
    private:
        std::pair<Object*, void*> mObj;
    };

    template <class T>
    union ServiceLocation {
        std::aligned_storage_t<sizeof(T), alignof(T)> x;
        auto& Get() { return *reinterpret_cast<T*>(&x); }
    };
}

#define NW_MAKE_SERVICE(T, URI, PRI, VAR) \
    ::NEWorld::ServiceLocation<T> VAR; NW_META_CLASS_ATTRIBUTE(T, ::NEWorld::ServiceInfo, URI, PRI, &(VAR))
