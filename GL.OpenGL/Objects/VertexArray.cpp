#include "Support.h"

namespace {
    OpenGL::LongPtr Fn[14];

    constexpr const char *Name =
            "glCreateVertexArrays\0glDisableVertexArrayAttrib\0glEnableVertexArrayAttrib\0"
            "glVertexArrayElementBuffer\0glVertexArrayVertexBuffer\0glVertexArrayVertexBuffers\0"
            "glVertexArrayAttribBinding\0glVertexArrayAttribFormat\0glVertexArrayAttribIFormat\0"
            "glVertexArrayAttribLFormat\0glVertexArrayBindingDivisor\0glGetVertexArrayiv\0"
            "glGetVertexArrayIndexediv\0glGetVertexArrayIndexed64iv\0";
}

namespace OpenGL {
    static_assert(std::is_standard_layout_v<VertexArray>);

    void VertexArray::Create() noexcept {
        Get<PFNGLCREATEVERTEXARRAYSPROC>(Fn[0])(1, &mHandle);
    }

    void VertexArray::DisableAttrib(GLuint index) noexcept {
        Get<PFNGLDISABLEVERTEXARRAYATTRIBPROC>(Fn[1])(mHandle, index);
    }

    void VertexArray::EnableAttrib(GLuint index) noexcept {
        Get<PFNGLENABLEVERTEXARRAYATTRIBPROC>(Fn[2])(mHandle, index);
    }

    void VertexArray::BindElementBuffer(GLuint buffer) noexcept {
        Get<PFNGLVERTEXARRAYELEMENTBUFFERPROC>(Fn[3])(mHandle, buffer);
    }

    void VertexArray::BindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) noexcept {
        Get<PFNGLVERTEXARRAYVERTEXBUFFERPROC>(Fn[4])(mHandle, bindingindex, buffer, offset, stride);
    }

    void VertexArray::BindVertexBuffers(GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets,
                                      const GLsizei *strides) noexcept {
        Get<PFNGLVERTEXARRAYVERTEXBUFFERSPROC>(Fn[5])(mHandle, first, count, buffers, offsets, strides);
    }

    void VertexArray::AttribBinding(GLuint attribindex, GLuint bindingindex) noexcept {
        Get<PFNGLVERTEXARRAYATTRIBBINDINGPROC>(Fn[6])(mHandle, attribindex, bindingindex);
    }

    void VertexArray::AttribFormat(GLuint attribindex, GLint size, GLenum type, GLboolean normalized,
            GLuint relativeoffset) noexcept {
        Get<PFNGLVERTEXARRAYATTRIBFORMATPROC>(Fn[7])(mHandle, attribindex, size, type, normalized, relativeoffset);
    }

    void VertexArray::AttribIFormat(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) noexcept {
        Get<PFNGLVERTEXARRAYATTRIBIFORMATPROC>(Fn[8])(mHandle, attribindex, size, type, relativeoffset);
    }

    void VertexArray::AttribLFormat(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) noexcept {
        Get<PFNGLVERTEXARRAYATTRIBLFORMATPROC>(Fn[9])(mHandle, attribindex, size, type, relativeoffset);
    }

    void VertexArray::BindingDivisor(GLuint bindingindex, GLuint divisor) noexcept {
        Get<PFNGLVERTEXARRAYBINDINGDIVISORPROC>(Fn[10])(mHandle, bindingindex, divisor);
    }

    void VertexArray::Getiv(GLenum pname, GLint *param) noexcept {
        Get<PFNGLGETVERTEXARRAYIVPROC>(Fn[11])(mHandle, pname, param);
    }

    void VertexArray::GetIndexediv(GLuint index, GLenum pname, GLint *param) noexcept {
        Get<PFNGLGETVERTEXARRAYINDEXEDIVPROC>(Fn[12])(mHandle, index, pname, param);
    }

    void VertexArray::GetIndexed64iv(GLuint index, GLenum pname, GLint64 *param) noexcept {
        Get<PFNGLGETVERTEXARRAYINDEXED64IVPROC>(Fn[13])(mHandle, index, pname, param);
    }

    VertexArray::~VertexArray() noexcept {

    }

    void InitVertexArray() { Load(Fn, Name); }
}