
#include "Objects.h"

#include "Support.h"

namespace {
    OpenGL::LongPtr Fn[13];

    constexpr const char *Name =
            "glCreateSamplers\0glDeleteSamplers\0glBindSampler\0glSamplerParameteri\0glSamplerParameteriv\0"
            "glSamplerParameterf\0glSamplerParameterfv\0glSamplerParameterIiv\0glSamplerParameterIuiv\0"
            "glGetSamplerParameteriv\0glGetSamplerParameterIiv\0glGetSamplerParameterfv\0glGetSamplerParameterIuiv\0";
}

namespace OpenGL {
    void Sampler::Create() noexcept {
        Get<PFNGLCREATESAMPLERSPROC>(Fn[0])(1, &mHandle);
    }

    void Sampler::BindTexture(GLuint unit) noexcept {
        Get<PFNGLBINDSAMPLERPROC>(Fn[2])(unit, mHandle);
    }

    void Sampler::Parameteri(GLenum pname, GLint param) noexcept {
        Get<PFNGLSAMPLERPARAMETERIPROC>(Fn[3])(mHandle, pname, param);
    }

    void Sampler::Parameteriv(GLenum pname, const GLint *param) noexcept {
        Get<PFNGLSAMPLERPARAMETERIVPROC>(Fn[4])(mHandle, pname, param);
    }

    void Sampler::Parameterf(GLenum pname, GLfloat param) noexcept {
        Get<PFNGLSAMPLERPARAMETERFPROC>(Fn[5])(mHandle, pname, param);
    }

    void Sampler::Parameterfv(GLenum pname, const GLfloat *param) noexcept {
        Get<PFNGLSAMPLERPARAMETERFVPROC>(Fn[6])(mHandle, pname, param);
    }

    void Sampler::ParameterIiv(GLenum pname, const GLint *param) noexcept {
        Get<PFNGLSAMPLERPARAMETERIIVPROC>(Fn[7])(mHandle, pname, param);
    }

    void Sampler::ParameterIuiv(GLenum pname, const GLuint *param) noexcept {
        Get<PFNGLSAMPLERPARAMETERIUIVPROC>(Fn[8])(mHandle, pname, param);
    }

    void Sampler::GetParameteriv(GLenum pname, GLint *params) noexcept {
        Get<PFNGLGETSAMPLERPARAMETERIVPROC>(Fn[9])(mHandle, pname, params);
    }

    void Sampler::GetParameterIiv(GLenum pname, GLint *params) noexcept {
        Get<PFNGLGETSAMPLERPARAMETERIIVPROC>(Fn[10])(mHandle, pname, params);
    }

    void Sampler::GetParameterfv(GLenum pname, GLfloat *params) noexcept {
        Get<PFNGLGETSAMPLERPARAMETERFVPROC>(Fn[11])(mHandle, pname, params);}

    void Sampler::GetParameterIuiv(GLenum pname, GLuint *params) noexcept {
        Get<PFNGLGETSAMPLERPARAMETERIUIVPROC>(Fn[12])(mHandle, pname, params);
    }

    Sampler::~Sampler() noexcept {
        Get<PFNGLDELETESAMPLERSPROC>(Fn[1])(1, &mHandle);
    }

    void InitSampler() { Load(Fn, Name); }
}