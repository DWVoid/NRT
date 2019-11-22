#pragma once

#include "CoreArbFn.h"
#include "OpenGL/Objects.h"
#include <type_traits>

namespace OpenGL {
    template <class T>
    constexpr T Get(LongPtr func) noexcept { return reinterpret_cast<T>(func); }

    void Load(LongPtr fnTable[], const char* names);

    void InitBuffer();

    void InitFramebuffer();

    void InitTexture();

    void InitVertexArray();
}