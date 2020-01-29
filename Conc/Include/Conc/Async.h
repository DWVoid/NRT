#pragma once

#include "Config.h"
#include "Cfx/Threading/Micro/Promise.h"

namespace InterOp {
	NRTCONC_API void AsyncResumePrevious() noexcept;

	NRTCONC_API IExecTask* AsyncGetCurrent() noexcept;

    NRTCONC_API void AsyncCall(IExecTask* inner) noexcept;

	NRTCONC_API void AsyncCallSync(IExecTask* inner) noexcept;
}

template <template <class> class Cont, class U>
U Await(Cont<U> cont) {
    const auto context = InterOp::AsyncGetCurrent();
    if (!context) {
        throw std::runtime_error("awaiting on none async context");
    }
    if constexpr (std::is_same_v<U, void>) {
        auto fu = cont.Then([task = context](auto&& lst) {
            ThreadPool::Enqueue(std::unique_ptr<IExecTask>(task));
            lst.Get();
        });
        InterOp::AsyncResumePrevious();
        fu.Get();
    }
    else {
        auto fu = cont.Then([task = context](auto&& lst) {
            ThreadPool::Enqueue(std::unique_ptr<IExecTask>(task));
            return lst.Get();
        });
        InterOp::AsyncResumePrevious();
        return fu.Get();
    };
}

template <class Func, class ...Ts>
auto Async(Func func, Ts&& ... args) {
    auto inner = Temp::New<InterOp::DeferredProcedureCallTask<std::decay_t<Func>, Ts...>>(
            std::forward<std::decay_t<Func>>(std::move(func)),
            std::forward<Ts>(args)...
    );
    auto future = inner->GetFuture();
    InterOp::AsyncCall(inner);
    return future;
}

template <class Func, class ...Ts>
auto AsyncLight(Func func, Ts&& ... args) {
    auto inner = Temp::New<InterOp::DeferredProcedureCallTask<std::decay_t<Func>, Ts...>>(
            std::forward<std::decay_t<Func>>(std::move(func)),
            std::forward<Ts>(args)...
    );
    auto future = inner->GetFuture();
    ThreadPool::Enqueue(inner);
    return future;
}

template <class Func, class ...Ts>
auto AsyncHere(Func func, Ts&& ... args) {
    auto inner = Temp::New<InterOp::DeferredProcedureCallTask<std::decay_t<Func>, Ts...>>(
            std::forward<std::decay_t<Func>>(std::move(func)),
            std::forward<Ts>(args)...
    );
    auto future = inner->GetFuture();
    InterOp::AsyncCallSync(inner);
    return future;
}
