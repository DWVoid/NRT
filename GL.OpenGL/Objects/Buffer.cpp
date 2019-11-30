#include "Support.h"

namespace {
    OpenGL::LongPtr Fn[15];

    constexpr const char *Name =
            "glCreateBuffers\0glNamedBufferStorage\0glDeleteBuffers\0glNamedBufferSubData\0"
            "glCopyNamedBufferSubData\0glClearNamedBufferData\0glClearNamedBufferSubData\0glMapNamedBuffer\0"
            "glMapNamedBufferRange\0glUnmapNamedBuffer\0glFlushMappedNamedBufferRange\0glGetNamedBufferParameteriv\0"
            "glGetNamedBufferParameteri64v\0glGetNamedBufferPointerv\0glGetNamedBufferSubData\0";
}

namespace OpenGL {
    Buffer::~Buffer() noexcept {
        Get<PFNGLDELETEBUFFERSPROC>(Fn[2])(1, &mHandle);
    }

    void Buffer::Create() noexcept {
        Get<PFNGLCREATEBUFFERSPROC>(Fn[0])(1, &mHandle);
    }

    void Buffer::Storage(GLsizeiptr size, const void *data, GLbitfield flags) noexcept {
        Get<PFNGLNAMEDBUFFERSTORAGEPROC>(Fn[1])(mHandle, size, data, flags);
    }

    void Buffer::SubData(GLintptr offset, GLsizeiptr size, const void *data) noexcept {
        Get<PFNGLNAMEDBUFFERSUBDATAPROC>(Fn[3])(mHandle, offset, size, data);
    }

    void Buffer::CopySubData(Buffer& read, Buffer& write,
                             GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) noexcept {
        Get<PFNGLCOPYNAMEDBUFFERSUBDATAPROC>(Fn[4])(read.mHandle, write.mHandle, readOffset, writeOffset, size);
    }

    void Buffer::ClearData(GLenum internalformat, GLenum format, GLenum type, const void *data) noexcept {
        Get<PFNGLCLEARNAMEDBUFFERDATAPROC>(Fn[5])(mHandle, internalformat, format, type, data);
    }

    void Buffer::ClearSubData(GLenum internalformat, GLintptr offset, GLsizeiptr size,
                              GLenum format, GLenum type, const void *data) noexcept {
        Get<PFNGLCLEARNAMEDBUFFERSUBDATAPROC>(Fn[6])(mHandle, internalformat, offset, size, format, type, data);
    }

    void *Buffer::Map(GLenum access) noexcept {
        return Get<PFNGLMAPNAMEDBUFFERPROC>(Fn[7])(mHandle, access);
    }

    void *Buffer::MapRange(GLintptr offset, GLsizeiptr length, GLbitfield access) noexcept {
        return Get<PFNGLMAPNAMEDBUFFERRANGEPROC>(Fn[8])(mHandle, offset, length, access);
    }

    GLboolean Buffer::Unmap() noexcept {
        return Get<PFNGLUNMAPNAMEDBUFFERPROC>(Fn[9])(mHandle);
    }

    void Buffer::FlushMappedRange(GLintptr offset, GLsizeiptr length) noexcept {
        Get<PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC>(Fn[10])(mHandle, offset, length);
    }

    void Buffer::GetParameteriv(GLenum pname, GLint *params) noexcept {
        Get<PFNGLGETNAMEDBUFFERPARAMETERIVPROC>(Fn[11])(mHandle, pname, params);
    }

    void Buffer::GetParameteri64v(GLenum pname, GLint64 *params) noexcept {
        Get<PFNGLGETNAMEDBUFFERPARAMETERI64VPROC>(Fn[12])(mHandle, pname, params);
    }

    void Buffer::GetPointerv(GLenum pname, void **params) noexcept {
        Get<PFNGLGETNAMEDBUFFERPOINTERVPROC>(Fn[13])(mHandle, pname, params);
    }

    void Buffer::GetSubData(GLintptr offset, GLsizeiptr size, void *data) noexcept {
        Get<PFNGLGETNAMEDBUFFERSUBDATAPROC>(Fn[14])(mHandle, offset, size, data);
    }

    void InitBuffer() { Load(Fn, Name); }
}
