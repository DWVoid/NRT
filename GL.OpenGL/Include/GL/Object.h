#pragma once

#include "Config.h"

namespace GL {
    struct Object {
        virtual ~Object() noexcept = default;
    protected:
        template <class T>
        T& down() noexcept { return *static_cast<T*>(this); }
        template <class T>
        const T& down() const noexcept { return *static_cast<const T*>(this); }
    };
}
