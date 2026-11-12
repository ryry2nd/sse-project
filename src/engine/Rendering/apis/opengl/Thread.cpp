#include "Thread.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

namespace {

struct Instance {
    std::mutex mutex;
    std::mutex shutdownMutex;
    std::condition_variable cv;

    std::queue<std::function<void()>> tasks;

    std::thread worker;
    std::thread::id workerId;

    bool stopping = false;

    Instance() {
        worker = std::thread([this] {
            for (;;) {
                std::function<void()> task;

                {
                    std::unique_lock lock(mutex);

                    cv.wait(lock, [this] {
                        return stopping || !tasks.empty();
                    });

                    if (stopping && tasks.empty())
                        break;

                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task();
            }
        });

        // Set before the constructor returns, so no thread can see an unset id.
        workerId = worker.get_id();
    }

    void shutdown() {
        if (std::this_thread::get_id() == workerId)
            return;

        std::lock_guard shutdownLock(shutdownMutex);

        {
            std::lock_guard lock(mutex);
            stopping = true;
        }

        cv.notify_all();

        if (worker.joinable())
            worker.join();
    }

    ~Instance() {
        shutdown();
    }
};

Instance& instance() {
    static Instance inst;
    return inst;
}

} // namespace

void StaticThread::init() {
    (void)instance();
}

void StaticThread::shutdown() {
    instance().shutdown();
}

bool StaticThread::onWorkerThread() {
    return std::this_thread::get_id() == instance().workerId;
}

void StaticThread::enqueue(std::function<void()> task) {
    auto& inst = instance();

    {
        std::lock_guard lock(inst.mutex);

        if (inst.stopping)
            throw std::runtime_error("StaticThread is shutting down");

        inst.tasks.push(std::move(task));
    }

    inst.cv.notify_one();
}