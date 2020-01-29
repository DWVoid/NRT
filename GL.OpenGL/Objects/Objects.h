#pragma once

#include "CoreArbDef.h"

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

        [[nodiscard]] GLuint Native() const noexcept { return mHandle; }

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

        ~Texture() noexcept;

        void Create(GLenum target) noexcept;

        void Buffer(GLenum internalformat, GLuint buffer) noexcept;

        void BufferRange(GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) noexcept;

        void Storage1D(GLsizei levels, GLenum internalformat, GLsizei width) noexcept;

        void Storage2D(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) noexcept;

        void
        Storage3D(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) noexcept;

        void Storage2DMultisample(GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height,
                                             GLboolean fixedsamplelocations) noexcept;

        void
        Storage3DMultisample(GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
                             GLboolean fixedsamplelocations) noexcept;

        void
        SubImage1D(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels) noexcept;

        void
        SubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type,
                   const void *pixels) noexcept;

        void
        SubImage3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height,
                   GLsizei depth, GLenum format, GLenum type, const void *pixels) noexcept;

        void
        CompressedSubImage1D(GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize,
                             const void *data) noexcept;

        void
        CompressedSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format,
                             GLsizei imageSize, const void *data) noexcept;

        void
        CompressedSubImage3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height,
                             GLsizei depth, GLenum format, GLsizei imageSize, const void *data) noexcept;

        void CopySubImage1D(GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) noexcept;

        void CopySubImage2D(GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width,
                                       GLsizei height) noexcept;

        void
        CopySubImage3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width,
                       GLsizei height) noexcept;

        void Parameterf(GLenum pname, GLfloat param) noexcept;

        void Parameterfv(GLenum pname, const GLfloat *param) noexcept;

        void Parameteri(GLenum pname, GLint param) noexcept;

        void ParameterIiv(GLenum pname, const GLint *params) noexcept;

        void ParameterIuiv(GLenum pname, const GLuint *params) noexcept;

        void Parameteriv(GLenum pname, const GLint *param) noexcept;

        void GenerateMipmap() noexcept;

        void BindUnit(GLuint unit) noexcept;

        void GetImage(GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels) noexcept;

        void GetCompressedImage(GLint level, GLsizei bufSize, void *pixels) noexcept;

        void GetLevelParameterfv(GLint level, GLenum pname, GLfloat *params) noexcept;

        void GetLevelParameteriv(GLint level, GLenum pname, GLint *params) noexcept;

        void GetParameterfv(GLenum pname, GLfloat *params) noexcept;

        void GetParameterIiv(GLenum pname, GLint *params) noexcept;

        void GetParameterIuiv(GLenum pname, GLuint *params) noexcept;

        void GetParameteriv(GLenum pname, GLint *params) noexcept;
    };

    class VertexArray final : public Handle {
    public:
        constexpr VertexArray() noexcept = default;

        explicit VertexArray(CreateTag) noexcept: Handle(NoInit) { Create(); }

        VertexArray(VertexArray &&) = default;

        VertexArray &operator=(VertexArray &&) = default;

        ~VertexArray() noexcept;

        void Create() noexcept;

        void DisableAttrib(GLuint index) noexcept;

        void EnableAttrib(GLuint index) noexcept;

        void BindElementBuffer(GLuint buffer) noexcept;

        void BindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) noexcept;

        void BindVertexBuffers(GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets,
                                          const GLsizei *strides) noexcept;

        void AttribBinding(GLuint attribindex, GLuint bindingindex) noexcept;

        void
        AttribFormat(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset) noexcept;

        void AttribIFormat(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) noexcept;

        void AttribLFormat(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset) noexcept;

        void BindingDivisor(GLuint bindingindex, GLuint divisor) noexcept;

        void Getiv(GLenum pname, GLint *param) noexcept;

        void GetIndexediv(GLuint index, GLenum pname, GLint *param) noexcept;

        void GetIndexed64iv(GLuint index, GLenum pname, GLint64 *param) noexcept;
    };

    class Buffer final : public Handle {
    public:
        constexpr Buffer() noexcept = default;

        explicit Buffer(CreateTag) noexcept: Handle(NoInit) { Create(); }

        Buffer(Buffer &&) = default;

        Buffer &operator=(Buffer &&) = default;

        ~Buffer() noexcept;

        void Create() noexcept;

        void Storage(GLsizeiptr size, const void *data, GLbitfield flags) noexcept;

        void SubData(GLintptr offset, GLsizeiptr size, const void *data) noexcept;

        static void CopySubData(Buffer& read, Buffer& write,
                                           GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) noexcept;

        void ClearData(GLenum internalformat, GLenum format, GLenum type, const void *data) noexcept;

        void ClearSubData(GLenum internalformat, GLintptr offset, GLsizeiptr size,
                                     GLenum format, GLenum type, const void *data) noexcept;

        void *Map(GLenum access) noexcept;

        void *MapRange(GLintptr offset, GLsizeiptr length, GLbitfield access) noexcept;

        GLboolean Unmap() noexcept;

        void FlushMappedRange(GLintptr offset, GLsizeiptr length) noexcept;

        void GetParameteriv(GLenum pname, GLint *params) noexcept;

        void GetParameteri64v(GLenum pname, GLint64 *params) noexcept;

        void GetPointerv(GLenum pname, void **params) noexcept;

        void GetSubData(GLintptr offset, GLsizeiptr size, void *data) noexcept;
    };

    class Framebuffer final : public Handle {
    public:
        constexpr Framebuffer() noexcept = default;

        explicit Framebuffer(CreateTag) noexcept: Handle(NoInit) { Create(); }

        Framebuffer(Framebuffer &&) = default;

        Framebuffer &operator=(Framebuffer &&) = default;

        ~Framebuffer() noexcept;

        void Create() noexcept;

        void Renderbuffer(GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) noexcept;

        void Parameteri(GLenum pname, GLint param) noexcept;

        void Texture(GLenum attachment, GLuint texture, GLint level) noexcept;

        void TextureLayer(GLenum attachment, GLuint texture, GLint level, GLint layer) noexcept;

        void DrawBuffer(GLenum buf) noexcept;

        void DrawBuffers(GLsizei n, const GLenum *bufs) noexcept;

        void ReadBuffer(GLenum src) noexcept;

        void InvalidateData(GLsizei numAttachments, const GLenum *attachments) noexcept;

        void InvalidateSubData(GLsizei numAttachments, const GLenum *attachments,
                                          GLint x, GLint y, GLsizei width, GLsizei height) noexcept;

        void Cleariv(GLenum buffer, GLint drawbuffer, const GLint *value) noexcept;

        void Clearuiv(GLenum buffer, GLint drawbuffer, const GLuint *value) noexcept;

        void Clearfv(GLenum buffer, GLint drawbuffer, const GLfloat *value) noexcept;

        void Clearfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) noexcept;

        static void Blit(GLuint readFramebuffer, GLuint drawFramebuffer,
                                    GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                                    GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask,
                                    GLenum filter) noexcept;

        GLenum CheckStatus(GLenum target) noexcept;

        void GetParameteriv(GLenum pname, GLint *param) noexcept;

        void GetAttachmentParameteriv(GLenum attachment, GLenum pname, GLint *params) noexcept;
    };

    class Shader final : public Handle {
    public:
        constexpr Shader() noexcept = default;

        explicit Shader(CreateTag, GLenum type) noexcept: Handle(NoInit) { Create(type); }

        Shader(Shader &&) = default;

        Shader &operator=(Shader &&) = default;

        ~Shader() noexcept;

        void Create(GLenum type) noexcept;

        void Binary(const void *data, GLsizei length) noexcept;

        void Specialize(const GLchar *pEntryPoint, GLuint numSpecializationConstants,
                                   const GLuint *pConstantIndex, const GLuint *pConstantValue) noexcept;

        void Getiv(GLenum pname, GLint *param) noexcept;

        void GetInfoLog(GLsizei bufSize, GLsizei *length, GLchar *infoLog) noexcept;
    };

    class Program final : public Handle {
    public:
        constexpr Program() noexcept = default;

        explicit Program(CreateTag) noexcept: Handle(NoInit) { Create(); }

        Program(Program &&) = default;

        Program &operator=(Program &&) = default;

        ~Program() noexcept;

        void Create() noexcept;

        void Link() noexcept;
        
        void Attach(Shader& shader) noexcept;
        
        void Detach(Shader& shader) noexcept;

        void Getiv(GLenum pname, GLint *param) noexcept;

        void GetInfoLog(GLsizei bufSize, GLsizei *length, GLchar *infoLog) noexcept;
    };

    class ProgramPipeline final : public Handle {
    public:
        constexpr ProgramPipeline() noexcept = default;

        // explicit ProgramPipeline(CreateTag) noexcept: Handle(NoInit) { Create(); }

        ProgramPipeline(ProgramPipeline &&) = default;

        ProgramPipeline &operator=(ProgramPipeline &&) = default;

        ~ProgramPipeline() noexcept;
    };

    class Sampler final : public Handle {
    public:
        constexpr Sampler() noexcept = default;

        explicit Sampler(CreateTag) noexcept: Handle(NoInit) { Create(); }

        Sampler(Sampler &&) = default;

        Sampler &operator=(Sampler &&) = default;

        ~Sampler() noexcept;

        void Create() noexcept;

        void BindTexture(GLuint unit) noexcept;

        void Parameteri(GLenum pname, GLint param) noexcept;

        void Parameteriv(GLenum pname, const GLint *param) noexcept;

        void Parameterf(GLenum pname, GLfloat param) noexcept;

        void Parameterfv(GLenum pname, const GLfloat *param) noexcept;

        void ParameterIiv(GLenum pname, const GLint *param) noexcept;

        void ParameterIuiv(GLenum pname, const GLuint *param) noexcept;

        void GetParameteriv(GLenum pname, GLint *params) noexcept;

        void GetParameterIiv(GLenum pname, GLint *params) noexcept;

        void GetParameterfv(GLenum pname, GLfloat *params) noexcept;

        void GetParameterIuiv(GLenum pname, GLuint *params) noexcept;
    };

    using LongPtr = void (*)() noexcept;

    using FunctionProvider = LongPtr(*)(const char *name, void *user) noexcept;

    void SetFunctionProvider(FunctionProvider provider, void *user) noexcept;

    void GetFunctionProvider(FunctionProvider &provider, void *&user) noexcept;

    void Initialize();

    GLenum Error() noexcept;
}
