#pragma once

#include <memory>
#include <vector>

class IExecTask {
public:
    virtual void Exec() noexcept = 0;
    virtual ~IExecTask() noexcept = default;
};

class AInstancedExecTask : public IExecTask{
public:
    void Exec() noexcept override;
    virtual void Exec(uint32_t instance) noexcept = 0;
};

class ThreadPool {
public:
    static bool LocalEnqueue(std::unique_ptr<IExecTask>& task) {
        return LocalEnqueue(task.release());
    }

    static void Enqueue(std::unique_ptr<IExecTask> task) {
        Enqueue(task.release());
    }

    static void Spawn(std::unique_ptr<AInstancedExecTask> task) {
        Enqueue(task.release());
    }

    static bool LocalEnqueue(IExecTask* task) noexcept;
    static void Enqueue(IExecTask* task) noexcept;
    static void Spawn(AInstancedExecTask* task) noexcept;
    static void Stop() noexcept;
    static void Panic() noexcept;
};