#include "Support.h"
#include "Objects.h"

#include <string_view>
#include <stdexcept>

namespace OpenGL {
    namespace {
        LongPtr Fn[1];
        constexpr const char* Name =
                "glGetError\0";
    }

    static FunctionProvider gProvider;
    static void *gProviderUser;

    void SetFunctionProvider(FunctionProvider provider, void *user) noexcept {
        gProvider = provider;
        gProviderUser = user;
    }

    void GetFunctionProvider(FunctionProvider &provider, void *&user) noexcept {
        provider = gProvider;
        user = gProviderUser;
    }

    void Load(LongPtr fnTable[], const char *names) {
        for (auto i = 0; names[0] != '\0'; ++i) {
            std::string_view thisName{names};
            names += thisName.size() + 1;
            fnTable[i] = gProvider(names, gProviderUser);
            if (!fnTable[i]) {
                throw std::runtime_error(std::string("Could not locate required function:") + std::string(thisName));
            }
        }
    }

    void Initialize() {
        Load(Fn, Name);
        InitBuffer();
        InitFramebuffer();
        InitTexture();
        InitSampler();
        InitShaders();
        InitVertexArray();
    }

    GLenum Error() noexcept { return Get<PFNGLGETERRORPROC>(Fn[0])(); }
}
