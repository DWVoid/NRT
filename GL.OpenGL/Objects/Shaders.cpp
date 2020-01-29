
#include "Objects.h"

#include "Support.h"

namespace {
    OpenGL::LongPtr Fn[18];

    constexpr const char *Name =
            "glCreateShader\0glShaderBinary\0glGetShaderiv\0glGetShaderInfoLog\0glDeleteShader\0"
            "glCreateProgram\0glProgramParameteri\0glLinkProgram\0glAttachShader\0glDetachShader\0"
            "glGetProgramiv\0glGetProgramInfoLog\0glDeleteProgram\0";
}

namespace OpenGL {
    void Shader::Create(GLenum type) noexcept {
        mHandle = Get<PFNGLCREATESHADERPROC>(Fn[0])(type);
    }

    void Shader::Binary(const void *data, GLsizei length) noexcept {
        Get<PFNGLSHADERBINARYPROC>(Fn[1])(1, &mHandle, GL_SHADER_BINARY_FORMAT_SPIR_V, data, length);
    }

    void Shader::Specialize(const GLchar *pEntryPoint, GLuint numSpecializationConstants, const GLuint *pConstantIndex,
                       const GLuint *pConstantValue) noexcept {
        Get<PFNGLSPECIALIZESHADERPROC>(Fn[2])(mHandle, pEntryPoint, numSpecializationConstants,
                                              pConstantIndex, pConstantValue);
    }

    void Shader::Getiv(GLenum pname, GLint *param) noexcept {
        Get<PFNGLGETSHADERIVPROC>(Fn[3])(mHandle, pname, param);
    }

    void Shader::GetInfoLog(GLsizei bufSize, GLsizei *length, GLchar *infoLog) noexcept {
        Get<PFNGLGETSHADERINFOLOGPROC>(Fn[4])(mHandle, bufSize, length, infoLog);
    }

    Shader::~Shader() noexcept {
        if (mHandle != 0) {
            Get<PFNGLDELETESHADERPROC>(Fn[5])(mHandle);
        }
    }

    void Program::Create() noexcept {
        mHandle = Get<PFNGLCREATEPROGRAMPROC>(Fn[6])();
        if (mHandle) {
            Get<PFNGLPROGRAMPARAMETERIPROC>(Fn[7])(mHandle, GL_PROGRAM_SEPARABLE, GL_TRUE);
        }
    }

    void Program::Link() noexcept {
        Get<PFNGLLINKPROGRAMPROC>(Fn[8])(mHandle);
    }

    void Program::Attach(Shader &shader) noexcept {
        Get<PFNGLATTACHSHADERPROC>(Fn[9])(mHandle, shader.Native());
    }

    void Program::Detach(Shader &shader) noexcept {
        Get<PFNGLDETACHSHADERPROC>(Fn[10])(mHandle, shader.Native());
    }

    void Program::Getiv(GLenum pname, GLint *param) noexcept {
        Get<PFNGLGETPROGRAMIVPROC>(Fn[11])(mHandle, pname, param);
    }

    void Program::GetInfoLog(GLsizei bufSize, GLsizei *length, GLchar *infoLog) noexcept {
        Get<PFNGLGETPROGRAMINFOLOGPROC>(Fn[12])(mHandle, bufSize, length, infoLog);
    }

    Program::~Program() noexcept {
        if (mHandle) {
            Get<PFNGLDELETEPROGRAMPROC>(Fn[13])(mHandle);
        }
    }

    void InitShaders() { Load(Fn, Name); }
}