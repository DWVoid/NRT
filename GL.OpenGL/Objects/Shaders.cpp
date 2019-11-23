
#include <OpenGL/Objects.h>

#include "Support.h"

namespace {
    OpenGL::LongPtr Fn[18];

    constexpr const char *Name =
            "glCreateShader\0glShaderBinary\0glGetShaderiv\0glGetShaderInfoLog\0glDeleteShader\0"
            "glCreateProgram\0";
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
    }

    void Program::Link() noexcept {
        Get<PFNGLLINKPROGRAMPROC>(Fn[7])(mHandle);
    }
}