#include "Core/Threading/Micro/Promise.h"

namespace {
    const char* GetErrorString(FutureErrorCode code) noexcept {
        switch (code) {
        case FutureErrorCode::BrokenPromise: return "FutureError:broken promise";
        case FutureErrorCode::FutureAlreadyRetrieved: return "FutureError:future already retrieved";
        case FutureErrorCode::PromiseAlreadySatisfied: return "FutureError:promise already satisfied";
        case FutureErrorCode::NoState: return "FutureError:no state";
        }
        return "";
    }
}

FutureError::FutureError(const FutureErrorCode ec): std::logic_error(GetErrorString(ec)), _Code(ec) {}