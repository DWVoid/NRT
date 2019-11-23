
#include <OpenGL/Objects.h>

#include "Support.h"

namespace {
    OpenGL::LongPtr Fn[33];

    constexpr const char *Name =
            "glTextureBuffer\0glTextureBufferRange\0glTextureStorage1D\0glTextureStorage2D\0glTextureStorage3D\0"
            "glTextureStorage2DMultisample\0glTextureStorage3DMultisample\0"
            "glTextureSubImage1D\0glTextureSubImage2D\0glTextureSubImage3D\0"
            "glCompressedTextureSubImage1D\0glCompressedTextureSubImage2D\0glCompressedTextureSubImage3D\0"
            "glCopyTextureSubImage1D\0glCopyTextureSubImage2D\0glCopyTextureSubImage3D\0"
            "glTextureParameterf\0glTextureParameterfv\0glTextureParameteri\0"
            "glTextureParameterIiv\0glTextureParameterIuiv\0glTextureParameteriv\0"
            "glGenerateTextureMipmap\0glBindTextureUnit\0glGetTextureImage\0glGetCompressedTextureImage\0"
            "glGetTextureLevelParameterfv\0glGetTextureLevelParameteriv\0glGetTextureParameterfv\0"
            "glGetTextureParameterIiv\0glGetTextureParameterIuiv\0glGetTextureParameteriv\0"
            "glCreateTextures\0";
}

namespace OpenGL {
    static_assert(std::is_standard_layout_v<Texture>);

    void Texture::Create(GLenum target) noexcept {
        Get<PFNGLCREATETEXTURESPROC>(Fn[32])(target, 1, &mHandle);
    }

    void Texture::Buffer(GLenum internalformat, GLuint buffer) noexcept {
        Get<PFNGLTEXTUREBUFFERPROC>(Fn[0])(mHandle, internalformat, buffer);
    }

    void Texture::BufferRange(GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) noexcept {
        Get<PFNGLTEXTUREBUFFERRANGEPROC>(Fn[1])(mHandle, internalformat, buffer, offset, size);
    }

    void Texture::Storage1D(GLsizei levels, GLenum internalformat, GLsizei width) noexcept {
        Get<PFNGLTEXTURESTORAGE1DPROC>(Fn[2])(mHandle, levels, internalformat, width);
    }

    void Texture::Storage2D(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) noexcept {
        Get<PFNGLTEXTURESTORAGE2DPROC>(Fn[3])(mHandle, levels, internalformat, width, height);
    }

    void Texture::Storage3D(GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height,
                            GLsizei depth) noexcept {
        Get<PFNGLTEXTURESTORAGE3DPROC>(Fn[4])(mHandle, levels, internalformat, width, height, depth);
    }

    void Texture::Storage2DMultisample(GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height,
                                       GLboolean fixedsamplelocations) noexcept {
        Get<PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC>(Fn[5])(mHandle, samples, internalformat,
                                                         width, height, fixedsamplelocations);
    }

    void Texture::Storage3DMultisample(GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height,
                                       GLsizei depth, GLboolean fixedsamplelocations) noexcept {
        Get<PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC>(Fn[6])(mHandle, samples, internalformat,
                                                         width, height, depth, fixedsamplelocations);
    }

    void Texture::SubImage1D(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type,
                             const void *pixels) noexcept {
        Get<PFNGLTEXTURESUBIMAGE1DPROC>(Fn[7])(mHandle, level, xoffset, width, format, type, pixels);
    }

    void Texture::SubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                             GLenum format, GLenum type, const void *pixels) noexcept {
        Get<PFNGLTEXTURESUBIMAGE2DPROC>(Fn[8])(mHandle, level, xoffset, yoffset, width, height,
                                               format, type, pixels);
    }

    void Texture::SubImage3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
                             GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type,
                             const void *pixels) noexcept {
        Get<PFNGLTEXTURESUBIMAGE3DPROC>(Fn[9])(mHandle, level, xoffset, yoffset, zoffset,
                                               width, height, depth, format, type, pixels);
    }

    void Texture::CompressedSubImage1D(GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize,
                                       const void *data) noexcept {
        Get<PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC>(Fn[10])(mHandle, level, xoffset, width, format,
                                                          imageSize, data);
    }

    void Texture::CompressedSubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                       GLenum format, GLsizei imageSize, const void *data) noexcept {
        Get<PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC>(Fn[11])(mHandle, level, xoffset, yoffset,
                                                          width, height, format, imageSize, data);
    }

    void Texture::CompressedSubImage3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
                                       GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize,
                                       const void *data) noexcept {
        Get<PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC>(Fn[12])(mHandle, level, xoffset, yoffset, zoffset,
                                                          width, height, depth, format, imageSize, data);
    }

    void Texture::CopySubImage1D(GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) noexcept {
        Get<PFNGLCOPYTEXTURESUBIMAGE1DPROC>(Fn[13])(mHandle, level, xoffset, x, y, width);
    }

    void Texture::CopySubImage2D(GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width,
                                 GLsizei height) noexcept {
        Get<PFNGLCOPYTEXTURESUBIMAGE2DPROC>(Fn[14])(mHandle, level, xoffset, yoffset, x, y, width, height);
    }

    void Texture::CopySubImage3D(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y,
                                 GLsizei width, GLsizei height) noexcept {
        Get<PFNGLCOPYTEXTURESUBIMAGE3DPROC>(Fn[15])(mHandle, level, xoffset, yoffset, zoffset, x, y, width, height);
    }

    void Texture::Parameterf(GLenum pname, GLfloat param) noexcept {
        Get<PFNGLTEXTUREPARAMETERFPROC>(Fn[16])(mHandle, pname, param);
    }

    void Texture::Parameterfv(GLenum pname, const GLfloat *param) noexcept {
        Get<PFNGLTEXTUREPARAMETERFVPROC>(Fn[17])(mHandle, pname, param);
    }

    void Texture::Parameteri(GLenum pname, GLint param) noexcept {
        Get<PFNGLTEXTUREPARAMETERIPROC>(Fn[18])(mHandle, pname, param);
    }

    void Texture::ParameterIiv(GLenum pname, const GLint *params) noexcept {
        Get<PFNGLTEXTUREPARAMETERIIVPROC>(Fn[19])(mHandle, pname, params);
    }

    void Texture::ParameterIuiv(GLenum pname, const GLuint *params) noexcept {
        Get<PFNGLTEXTUREPARAMETERIUIVPROC>(Fn[20])(mHandle, pname, params);
    }

    void Texture::Parameteriv(GLenum pname, const GLint *param) noexcept {
        Get<PFNGLTEXTUREPARAMETERIVPROC>(Fn[21])(mHandle, pname, param);
    }

    void Texture::GenerateMipmap() noexcept {
        Get<PFNGLGENERATETEXTUREMIPMAPPROC>(Fn[22])(mHandle);
    }

    void Texture::BindUnit(GLuint unit) noexcept {
        Get<PFNGLBINDTEXTUREUNITPROC>(Fn[23])(unit, mHandle);
    }

    void Texture::GetImage(GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels) noexcept {
        Get<PFNGLGETTEXTUREIMAGEPROC>(Fn[24])(mHandle, level, format, type, bufSize, pixels);
    }

    void Texture::GetCompressedImage(GLint level, GLsizei bufSize, void *pixels) noexcept {
        Get<PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC>(Fn[25])(mHandle, level, bufSize, pixels);
    }

    void Texture::GetLevelParameterfv(GLint level, GLenum pname, GLfloat *params) noexcept {
        Get<PFNGLGETTEXTURELEVELPARAMETERFVPROC>(Fn[26])(mHandle, level, pname, params);
    }

    void Texture::GetLevelParameteriv(GLint level, GLenum pname, GLint *params) noexcept {
        Get<PFNGLGETTEXTURELEVELPARAMETERIVPROC>(Fn[27])(mHandle, level, pname, params);
    }

    void Texture::GetParameterfv(GLenum pname, GLfloat *params) noexcept {
        Get<PFNGLGETTEXTUREPARAMETERFVPROC>(Fn[28])(mHandle, pname, params);
    }

    void Texture::GetParameterIiv(GLenum pname, GLint *params) noexcept {
        Get<PFNGLGETTEXTUREPARAMETERIIVPROC>(Fn[29])(mHandle, pname, params);
    }

    void Texture::GetParameterIuiv(GLenum pname, GLuint *params) noexcept {
        Get<PFNGLGETTEXTUREPARAMETERIUIVPROC>(Fn[30])(mHandle, pname, params);
    }

    void Texture::GetParameteriv(GLenum pname, GLint *params) noexcept {
        Get<PFNGLGETTEXTUREPARAMETERIVPROC>(Fn[31])(mHandle, pname, params);
    }

    Texture::~Texture() noexcept {

    }

    void InitTexture() { Load(Fn, Name); }
}
