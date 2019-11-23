
#include <OpenGL/Objects.h>

#include "Support.h"

namespace {
    OpenGL::LongPtr Fn[19];

    constexpr const char *Name =
            "glCreateFramebuffers\0glNamedFramebufferRenderbuffer\0glNamedFramebufferParameteri\0"
            "glNamedFramebufferTexture\0glNamedFramebufferTextureLayer\0glNamedFramebufferDrawBuffer\0"
            "glNamedFramebufferDrawBuffers\0glNamedFramebufferReadBuffer\0glInvalidateNamedFramebufferData\0"
            "glInvalidateNamedFramebufferSubData\0glClearNamedFramebufferiv\0glClearNamedFramebufferuiv\0"
            "glClearNamedFramebufferfv\0glClearNamedFramebufferfi\0glBlitNamedFramebuffer\0"
            "glCheckNamedFramebufferStatus\0glGetNamedFramebufferParameteriv\0"
            "glGetNamedFramebufferAttachmentParameteriv\0glDeleteFramebuffers\0";
}

namespace OpenGL {
    void Framebuffer::Create() noexcept {
        Get<PFNGLCREATEFRAMEBUFFERSPROC>(Fn[0])(1, &mHandle);
    }

    void Framebuffer::Renderbuffer(GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) noexcept {
        Get<PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC>(Fn[1])(mHandle, attachment, renderbuffertarget, renderbuffer);
    }

    void Framebuffer::Parameteri(GLenum pname, GLint param) noexcept {
        Get<PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC>(Fn[2])(mHandle, pname, param);
    }

    void Framebuffer::Texture(GLenum attachment, GLuint texture, GLint level) noexcept {
        Get<PFNGLNAMEDFRAMEBUFFERTEXTUREPROC>(Fn[3])(mHandle, attachment, texture, level);
    }

    void Framebuffer::TextureLayer(GLenum attachment, GLuint texture, GLint level, GLint layer) noexcept {
        Get<PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC>(Fn[4])(mHandle, attachment, texture, level, layer);
    }

    void Framebuffer::DrawBuffer(GLenum buf) noexcept {
        Get<PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC>(Fn[5])(mHandle, buf);
    }

    void Framebuffer::DrawBuffers(GLsizei n, const GLenum *bufs) noexcept {
        Get<PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC>(Fn[6])(mHandle, n, bufs);
    }

    void Framebuffer::ReadBuffer(GLenum src) noexcept {
        Get<PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC>(Fn[7])(mHandle, src);
    }

    void Framebuffer::InvalidateData(GLsizei numAttachments, const GLenum *attachments) noexcept {
        Get<PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC>(Fn[8])(mHandle, numAttachments, attachments);
    }

    void Framebuffer::InvalidateSubData(GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y,
                                        GLsizei width, GLsizei height) noexcept {
        Get<PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC>(Fn[9])(mHandle, numAttachments, attachments, x, y, width,
                                                               height);
    }

    void Framebuffer::Cleariv(GLenum buffer, GLint drawbuffer, const GLint *value) noexcept {
        Get<PFNGLCLEARNAMEDFRAMEBUFFERIVPROC>(Fn[10])(mHandle, buffer, drawbuffer, value);
    }

    void Framebuffer::Clearuiv(GLenum buffer, GLint drawbuffer, const GLuint *value) noexcept {
        Get<PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC>(Fn[11])(mHandle, buffer, drawbuffer, value);
    }

    void Framebuffer::Clearfv(GLenum buffer, GLint drawbuffer, const GLfloat *value) noexcept {
        Get<PFNGLCLEARNAMEDFRAMEBUFFERFVPROC>(Fn[12])(mHandle, buffer, drawbuffer, value);
    }

    void Framebuffer::Clearfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) noexcept {
        Get<PFNGLCLEARNAMEDFRAMEBUFFERFIPROC>(Fn[13])(mHandle, buffer, drawbuffer, depth, stencil);
    }

    void Framebuffer::Blit(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0,
                           GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                           GLbitfield mask,
                           GLenum filter) noexcept {
        Get<PFNGLBLITNAMEDFRAMEBUFFERPROC>(Fn[14])(readFramebuffer, drawFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0,
                                                   dstY0, dstX1, dstY1, mask, filter);
    }

    GLenum Framebuffer::CheckStatus(GLenum target) noexcept {
        return Get<PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC>(Fn[15])(mHandle, target);
    }

    void Framebuffer::GetParameteriv(GLenum pname, GLint *param) noexcept {
        Get<PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC>(Fn[16])(mHandle, pname, param);
    }

    void Framebuffer::GetAttachmentParameteriv(GLenum attachment, GLenum pname, GLint *params) noexcept {
        Get<PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC>(Fn[17])(mHandle, attachment, pname, params);
    }

    Framebuffer::~Framebuffer() noexcept {
        Get<PFNGLDELETEFRAMEBUFFERSPROC>(Fn[18])(1, &mHandle);
    }

    void InitFramebuffer() { Load(Fn, Name); }
}
