#include "Support.h"
#include <string_view>
#include <stdexcept>

namespace OpenGL {
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
        InitBuffer();
        InitFramebuffer();
        InitTexture();
        InitSampler();
        InitVertexArray();
    }
}
