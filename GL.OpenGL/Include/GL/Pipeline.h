#pragma once

#include <cstddef>
#include "Object.h"

namespace GL {
    enum class ShaderType: int {
        Vertex = 1, Fragment, Geometry, TesslationControl, TesslationEval, Compute
    };

    struct ShaderCreateInfo {
        std::byte* SpirVByteCode;
        const char* EntryPointName;
        int CodeSize;
        ShaderType Type;
    };

    struct ShaderModule: Object {
        NRTOGL_API static ShaderModule* Create(const ShaderCreateInfo& createInfo);
    };

    struct PipelineCreateInfo {
        ShaderModule* ShaderModules;
        int ShaderModuleCount;
    };

    struct Pipeline: Object {
        NRTOGL_API static Pipeline* Create(const PipelineCreateInfo& createInfo);
    };
}