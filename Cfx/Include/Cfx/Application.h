#pragma once

#include "Meta.h"

namespace NEWorld {
    class Application : public NEWorld::Object {
    public:
        NRTCRT_API Application() noexcept;
        NRTCRT_API ~Application() noexcept override;
    };
}
