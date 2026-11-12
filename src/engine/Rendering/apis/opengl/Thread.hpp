#pragma once

#include <functional>
#include <future>
#include <memory>
#include <utility>

class StaticThread {
public:
    static void init();
    static void shutdown();

    // True if the calling thread is the static worker thread.
    static bool onWorkerThread();

    // Queues a task on the worker thread. Throws if shutting down.
    static void enqueue(std::function<void()> task);

    // Runs fn on the worker thread, blocks until done, and returns its result.
    // Exceptions thrown by fn are rethrown in the caller.
    template<typename F>
    static auto run(F&& fn) -> decltype(fn()) {
        using R = decltype(fn());

        if (onWorkerThread())
            return fn();

        auto task = std::make_shared<std::packaged_task<R()>>(std::forward<F>(fn));
        auto future = task->get_future();

        enqueue([task] { (*task)(); });

        return future.get();
    }
};

// Only valid in functions returning void.
// A `return;` inside only exits the lambda, not the enclosing function.
#define RUN_IN_STATIC_THREAD \
    { StaticThread::run([&]() {

#define END_STATIC_THREAD \
    }); }