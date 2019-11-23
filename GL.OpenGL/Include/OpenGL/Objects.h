#pragma once

#include "Config.h"
#include <OpenGL/CoreArbDef.h>

namespace OpenGL {
    constexpr struct CreateTag {} Create{};

    class Handle {
    public:
        constexpr Handle() noexcept: mHandle(0) {}

        Handle(const Handle &) = delete;

        Handle &operator=(const Handle &) = delete;

        Handle(Handle &&r) noexcept: mHandle(r.mHandle) { r.mHandle = 0; }

        Handle &operator=(Handle &&r) noexcept {
            if (&r != this) {
                mHandle = r.mHandle;
                r.mHandle = 0;
            }
            return *this;
        }

    protected:
        GLuint mHandle;
        static constexpr struct NoInitTag {} NoInit{};

        constexpr explicit Handle(NoInitTag) noexcept: mHandle{} {} // NO INIT
    };

    class Texture final : public Handle {
    public:
        constexpr Texture() noexcept = default;

        explicit Texture(CreateTag, GLenum target) noexcept: Handle(NoInit) { Create(target); }

        Texture(Texture &&) = default;

        Texture &operator=(Texture &&) = default;

        NRTOGL_API ~Texture() noexcept;

        NRTOGL_API void Create(GLenum target) noexcept;

        NRTOGL_API void Buffer(GLenum internalformat, GLuint buffer) noexcept;

        NRTOGL_API void BufferRange(GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) noexcept;

        NRTOGL_API void Storage1D(GLsizei levels, GLenum internalformat, GLsizei width) noexcept;

        NRTOGL_API void Storage2D(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) noexcept;

        NRTOGL_API void
        Storage3D(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) noexcept;

        NRTOGL_API void Storage2DMultisample(GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height,
                                             GLboolean fixedsamplelocations) noexcept;

        NRTOGL_API void
        Storage3DMultisample(GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
                             GLboolean fixedsamplelocations) noexcept;

        NRTOGL_API void
        SubImage1D(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels) noexcept;

        NRTOGL_API void
        SubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type,
                   const void *pixels) noexcept;

        NRTOGL_API void
        SubImage3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height,
                   GLsizei depth, GLenum format, GLenum type, const void *pixels) noexcept;

        NRTOGL_API void
        CompressedSubImage1D(GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize,
                             const void *data) noexcept;

        NRTOGL_API void
        CompressedSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format,
                             GLsizei imageSize, const void *data) noexcept;

        NRTOGL_API void
        CompressedSubImage3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height,
                             GLsizei depth, GLenum format, GLsizei imageSize, const void *data) noexcept;

        NRTOGL_API void CopySubImage1D(GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) noexcept;

        NRTOGL_API void CopySubImage2D(GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width,
                                       GLsizei height) noexcept;

        NRTOGL_API void
        CopySubImage3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width,
                       GLsizei height) noexcept;

        NRTOGL_API void Parameterf(GLenum pname, GLfloat param) noexcept;

        NRTOGL_API void Parameterfv(GLenum pname, const GLfloat *param) noexcept;

        NRTOGL_API void Parameteri(GLenum pname, GLint param) noexcept;

        NRTOGL_API void ParameterIiv(GLenum pname, const GLint *params) noexcept;

        NRTOGL_API void ParameterIuiv(GLenum pname, const GLuint *params) noexcept;

        NRTOGL_API void Parameteriv(GLenum pname, const GLint *param) noexcept;

        NRTOGL_API void GenerateMipmap() noexcept;

        NRTOGL_API void BindUnit(GLuint unit) noexcept;

        NRTOGL_API void GetImage(GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels) noexcept;

        NRTOGL_API void GetCompressedImage(GLint level, GLsizei bufSize, void *pixels) noexcept;

        NRTOGL_API void GetLevelParameterfv(GLint level, GLenum pname, GLfloat *params) noexcept;

        NRTOGL_API void GetLevelParameteriv(GLint level, GLenum pname, GLint *params) noexcept;

        NRTOGL_API void GetParameterfv(GLenum pname, GLfloat *params) noexcept;

        NRTOGL_API void GetParameterIiv(GLenum pname, GLint *params) noexcept;

        NRTOGL_API void GetParameterIuiv(GLenum pname, GLuint *params) noexcept;

        NRTOGL_API void GetParameteriv(GLenum pname, GLint *params) noexcept;
    };

    class VertexArray final : public Handle {
    public:
        constexpr VertexArray() noexcept = default;

        explicit VertexArray(CreateTag) noexcept: Handle(NoInit) { Create(); }

        VertexArray(VertexArray &&) = default;

        VertexArray &operator=(VertexArray &&) = default;

        NRTOGL_API ~VertexArray() noexcept;

        NRTOGL_API void Create() noexcept;

        NRTOGL_API void DisableAttrib(GLuint index) noexcept;

        NRTOGL_API void EnableAttrib(GLuint index) noexcept;

        NRTOGL_API void BindElementBuffer(GLuint buffer) noexcept;

        NRTOGL_API void BindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) noexcept;

        NRTOGL_API void BindVertexBuffers(GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets,
                                          const GLsizei *strides) noexcept;

        NRTOGL_API void AttribBinding(GLuint attribindex, GLuint bindingindex) noexcept;

        NRTOGL_API void
        AttribFormat(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset) noexcept;

        NRTOGL_API void AttribIFormat(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) noexcept;

        NRTOGL_API void AttribLFormat(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) noexcept;

        NRTOGL_API void BindingDivisor(GLuint bindingindex, GLuint divisor) noexcept;

        NRTOGL_API void Getiv(GLenum pname, GLint *param) noexcept;

        NRTOGL_API void GetIndexediv(GLuint index, GLenum pname, GLint *param) noexcept;

        NRTOGL_API void GetIndexed64iv(GLuint index, GLenum pname, GLint64 *param) noexcept;
    };

    class Buffer final : public Handle {
    public:
        constexpr Buffer() noexcept = default;

        explicit Buffer(CreateTag) noexcept: Handle(NoInit) { Create(); }

        Buffer(Buffer &&) = default;

        Buffer &operator=(Buffer &&) = default;

        NRTOGL_API ~Buffer() noexcept;

        NRTOGL_API void Create() noexcept;

        NRTOGL_API void Storage(GLsizeiptr size, const void *data, GLbitfield flags) noexcept;

        NRTOGL_API void SubData(GLintptr offset, GLsizeiptr size, const void *data) noexcept;

        NRTOGL_API static void CopySubData(GLuint readBuffer, GLuint writeBuffer,
                                           GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) noexcept;

        NRTOGL_API void ClearData(GLenum internalformat, GLenum format, GLenum type, const void *data) noexcept;

        NRTOGL_API void ClearSubData(GLenum internalformat, GLintptr offset, GLsizeiptr size,
                                     GLenum format, GLenum type, const void *data) noexcept;

        NRTOGL_API void *Map(GLenum access) noexcept;

        NRTOGL_API void *MapRange(GLintptr offset, GLsizeiptr length, GLbitfield access) noexcept;

        NRTOGL_API GLboolean Unmap() noexcept;

        NRTOGL_API void FlushMappedRange(GLintptr offset, GLsizeiptr length) noexcept;

        NRTOGL_API void GetParameteriv(GLenum pname, GLint *params) noexcept;

        NRTOGL_API void GetParameteri64v(GLenum pname, GLint64 *params) noexcept;

        NRTOGL_API void GetPointerv(GLenum pname, void **params) noexcept;

        NRTOGL_API void GetSubData(GLintptr offset, GLsizeiptr size, void *data) noexcept;
    };

    class Framebuffer final : public Handle {
    public:
        constexpr Framebuffer() noexcept = default;

        explicit Framebuffer(CreateTag) noexcept: Handle(NoInit) { Create(); }

        Framebuffer(Framebuffer &&) = default;

        Framebuffer &operator=(Framebuffer &&) = default;

        NRTOGL_API ~Framebuffer() noexcept;

        NRTOGL_API void Create() noexcept;

        NRTOGL_API void Renderbuffer(GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) noexcept;

        NRTOGL_API void Parameteri(GLenum pname, GLint param) noexcept;

        NRTOGL_API void Texture(GLenum attachment, GLuint texture, GLint level) noexcept;

        NRTOGL_API void TextureLayer(GLenum attachment, GLuint texture, GLint level, GLint layer) noexcept;

        NRTOGL_API void DrawBuffer(GLenum buf) noexcept;

        NRTOGL_API void DrawBuffers(GLsizei n, const GLenum *bufs) noexcept;

        NRTOGL_API void ReadBuffer(GLenum src) noexcept;

        NRTOGL_API void InvalidateData(GLsizei numAttachments, const GLenum *attachments) noexcept;

        NRTOGL_API void InvalidateSubData(GLsizei numAttachments, const GLenum *attachments,
                                          GLint x, GLint y, GLsizei width, GLsizei height) noexcept;

        NRTOGL_API void Cleariv(GLenum buffer, GLint drawbuffer, const GLint *value) noexcept;

        NRTOGL_API void Clearuiv(GLenum buffer, GLint drawbuffer, const GLuint *value) noexcept;

        NRTOGL_API void Clearfv(GLenum buffer, GLint drawbuffer, const GLfloat *value) noexcept;

        NRTOGL_API void Clearfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) noexcept;

        NRTOGL_API static void Blit(GLuint readFramebuffer, GLuint drawFramebuffer,
                                    GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                    GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask,
                                    GLenum filter) noexcept;

        NRTOGL_API GLenum CheckStatus(GLenum target) noexcept;

        NRTOGL_API void GetParameteriv(GLenum pname, GLint *param) noexcept;

        NRTOGL_API void GetAttachmentParameteriv(GLenum attachment, GLenum pname, GLint *params) noexcept;
    };

    class Shader final : public Handle {
    public:
        constexpr Shader() noexcept = default;

        explicit Shader(CreateTag, GLenum type) noexcept: Handle(NoInit) { Create(type); }

        Shader(Shader &&) = default;

        Shader &operator=(Shader &&) = default;

        NRTOGL_API ~Shader() noexcept;

        NRTOGL_API void Create(GLenum type) noexcept;

        NRTOGL_API void Binary(const void *data, GLsizei length) noexcept;

        NRTOGL_API void Specialize(const GLchar *pEntryPoint, GLuint numSpecializationConstants,
                                   const GLuint *pConstantIndex, const GLuint *pConstantValue) noexcept;

        NRTOGL_API void Getiv(GLenum pname, GLint *param) noexcept;

        NRTOGL_API void GetInfoLog(GLsizei bufSize, GLsizei *length, GLchar *infoLog) noexcept;
    };

    class Program final : public Handle {
    public:
        constexpr Program() noexcept = default;

        explicit Program(CreateTag) noexcept: Handle(NoInit) { Create(); }

        Program(Program &&) = default;

        Program &operator=(Program &&) = default;

        NRTOGL_API ~Program() noexcept;

        NRTOGL_API void Create() noexcept;

        NRTOGL_API void Link() noexcept;
    };

    class ProgramPipeline final : public Handle {
    public:
        constexpr ProgramPipeline() noexcept = default;

        explicit ProgramPipeline(CreateTag) noexcept: Handle(NoInit) { Create(); }

        ProgramPipeline(ProgramPipeline &&) = default;

        ProgramPipeline &operator=(ProgramPipeline &&) = default;

        NRTOGL_API ~ProgramPipeline() noexcept;
    };

    class Sampler final : public mHandle {
    public:
        constexpr Sampler() noexcept = default;

        explicit Sampler(CreateTag) noexcept: Handle(NoInit) { Create(); }

        Sampler(Sampler &&) = default;

        Sampler &operator=(Sampler &&) = default;

        NRTOGL_API ~Sampler() noexcept;

        NRTOGL_API void Create() noexcept;

        NRTOGL_API void BindTexture(GLuint unit) noexcept;

        NRTOGL_API void Parameteri(GLenum pname, GLint param) noexcept;

        NRTOGL_API void Parameteriv(GLenum pname, const GLint *param) noexcept;

        NRTOGL_API void Parameterf(GLenum pname, GLfloat param) noexcept;

        NRTOGL_API void Parameterfv(GLenum pname, const GLfloat *param) noexcept;

        NRTOGL_API void ParameterIiv(GLenum pname, const GLint *param) noexcept;

        NRTOGL_API void ParameterIuiv(GLenum pname, const GLuint *param) noexcept;

        NRTOGL_API void GetParameteriv(GLenum pname, GLint *params) noexcept;

        NRTOGL_API void GetParameterIiv(GLenum pname, GLint *params) noexcept;

        NRTOGL_API void GetParameterfv(GLenum pname, GLfloat *params) noexcept;

        NRTOGL_API void GetParameterIuiv(GLenum pname, GLuint *params) noexcept;
    };

    using LongPtr = void (*)() noexcept;

    using FunctionProvider = LongPtr(*)(const char *name, void *user) noexcept;

    NRTOGL_API void SetFunctionProvider(FunctionProvider provider, void *user) noexcept;

    NRTOGL_API void GetFunctionProvider(FunctionProvider &provider, void *&user) noexcept;

    NRTOGL_API void Initialize();
}
