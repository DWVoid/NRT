#pragma once

#include <memory>
#include "Meta.h"
#include "Cfx/Config.h"

class IExecTask: public NEWorld::Object {
public:
    virtual void Exec() noexcept = 0;
};

class AInstancedExecTask : public IExecTask {
public:
    NRTCORE_API void Exec() noexcept override;
    virtual void Exec(uint32_t instance) noexcept = 0;
};

class ThreadPool {
public:
    static bool LocalEnqueue(std::unique_ptr<IExecTask>& task) { return LocalEnqueue(task.release()); }

    static void Enqueue(std::unique_ptr<IExecTask> task) { Enqueue(task.release()); }

    static void Spawn(std::unique_ptr<AInstancedExecTask> task) { Enqueue(task.release()); }

    NRTCORE_API static bool LocalEnqueue(IExecTask* task) noexcept;

    NRTCORE_API static void Enqueue(IExecTask* task) noexcept;

    NRTCORE_API static void Spawn(AInstancedExecTask* task) noexcept;

    NRTCORE_API static void Panic() noexcept;

    NRTCORE_API static int CountThreads() noexcept;
};
