#include "Cfx/Application.h"
#include "Service.h"

namespace {
    struct Control {
        NEWorld::ServiceHandle Timer{"org.newinfinideas.nrt.cfx.timer"};
        NEWorld::ServiceHandle Alloc{"org.newinfinideas.nrt.cfx.temp_alloc"};
        NEWorld::ServiceHandle Pool{"org.newinfinideas.nrt.cfx.thread_pool"};
    };

    NEWorld::ServiceLocation<Control> Ctrl;
}

namespace NEWorld {
    NEWorld::Application::Application() noexcept {
        new (&Ctrl.x) Control;
    }

    Application::~Application() noexcept {
        Ctrl.Get().~Control();
    }
}
