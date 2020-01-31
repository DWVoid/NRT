#pragma once

#include "Meta.h"
#include "Cfx/Config.h"

namespace NEWorld {
    class Application : public NEWorld::Object {
    public:
        NRTCORE_API Application() noexcept;
        NRTCORE_API ~Application() noexcept override;
    };
}
