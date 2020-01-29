#include <cassert>
#include <vector>
#include <stdexcept>
#include "GL/Pipeline.h"
#include "../Objects/Objects.h"

namespace GL {
    namespace {
        struct ShaderModuleImpl : ShaderModule {
            explicit ShaderModuleImpl(OpenGL::Program &&program, const ShaderType type) noexcept
                    : mProgram(std::move(program)), mType(type) {}

        private:
            OpenGL::Program mProgram;
            ShaderType mType;
        };

        constexpr GLenum TranslateType(const ShaderType type) noexcept {
            switch (type) {
                case ShaderType::Vertex:
                    return GL_VERTEX_SHADER;
                case ShaderType::Fragment:
                    return GL_FRAGMENT_SHADER;
                case ShaderType::Geometry:
                    return GL_GEOMETRY_SHADER;
                case ShaderType::TesslationControl:
                    return GL_TESS_CONTROL_SHADER;
                case ShaderType::TesslationEval:
                    return GL_TESS_EVALUATION_SHADER;
                case ShaderType::Compute:
                    return GL_COMPUTE_SHADER;
            }
            assert(false);
            return 0;
        }

        constexpr GLbitfield TranslateTypeBit(const ShaderType type) noexcept {
            switch (type) {
                case ShaderType::Vertex:
                    return GL_VERTEX_SHADER_BIT;
                case ShaderType::Fragment:
                    return GL_FRAGMENT_SHADER_BIT;
                case ShaderType::Geometry:
                    return GL_GEOMETRY_SHADER_BIT;
                case ShaderType::TesslationControl:
                    return GL_TESS_CONTROL_SHADER_BIT;
                case ShaderType::TesslationEval:
                    return GL_TESS_EVALUATION_SHADER_BIT;
                case ShaderType::Compute:
                    return GL_COMPUTE_SHADER_BIT;
            }
            assert(false);
            return 0;
        }

        [[noreturn]] void ShaderLoadingFailure(OpenGL::Shader &shader) {
            GLint maxLength{};
            shader.Getiv(GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<GLchar> infoLog(maxLength);
            shader.GetInfoLog(maxLength, &maxLength, &infoLog[0]);
            throw std::runtime_error(infoLog.data());
        }

        [[noreturn]] void ProgramLinkFailure(OpenGL::Program &program) {
            GLint maxLength{};
            program.Getiv(GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<GLchar> infoLog(maxLength);
            program.GetInfoLog(maxLength, &maxLength, &infoLog[0]);
            throw std::runtime_error(infoLog.data());
        }
    }

    ShaderModule *ShaderModule::Create(const ShaderCreateInfo &createInfo) {
        const auto glType = TranslateType(createInfo.Type);
        OpenGL::Shader shader{OpenGL::Create, glType};
        shader.Binary(createInfo.SpirVByteCode, createInfo.CodeSize);
        shader.Specialize(createInfo.EntryPointName, 0, nullptr, nullptr);
        GLint isCompiled{};
        shader.Getiv(GL_COMPILE_STATUS, &isCompiled);
        if (!isCompiled) ShaderLoadingFailure(shader);
        OpenGL::Program program{OpenGL::Create};
        program.Attach(shader);
        program.Link();
        program.Detach(shader);
        GLint isLinked{};
        program.Getiv(GL_LINK_STATUS, &isLinked);
        if (!isLinked) ProgramLinkFailure(program);
        return new ShaderModuleImpl(std::move(program), createInfo.Type);
    }

    Pipeline *Pipeline::Create(const PipelineCreateInfo &createInfo) {
        return nullptr;
    }
}
