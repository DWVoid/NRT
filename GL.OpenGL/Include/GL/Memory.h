#pragma once

#include "Object.h"

namespace GL {
    enum class BufferAccess: int {
        Read = 1, Write = 0b10, ReadWrite = 0b11
    };

    enum class BufferMappingOptions: int {
        Coherent = 1, InvalidateRange = 0b10, InvalidateBuffer = 0b100,
        ExplicitFlushing = 0b1000, NoSync = 0b10000
    };

    struct ClientBuffer: Object {
        NRTOGL_API void* Map(BufferAccess access);
        NRTOGL_API void* MapRegion(int start, int length, BufferAccess access, BufferMappingOptions options);
        NRTOGL_API void FlushRegion(int start, int length);
        NRTOGL_API bool Unmap();
        NRTOGL_API static ClientBuffer* Create(int size);
    };

    struct DeviceBuffer: Object {
        NRTOGL_API void Upload(ClientBuffer& source, int srcOffset, int dstOffset, int size);
        NRTOGL_API void Download(ClientBuffer& target, int srcOffset, int dstOffset, int size);
        NRTOGL_API void CopyFrom(DeviceBuffer& source, int srcOffset, int dstOffset, int size);
        NRTOGL_API static DeviceBuffer* Create(int size);
    };
}