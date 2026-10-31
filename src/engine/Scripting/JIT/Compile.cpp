#include <cstring>
#include <future>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "HeadJIT.hpp"
#include "HeadJITUnusable.hpp"

using namespace Engine::Scripting::Internal::Unusable;
using namespace Engine::Scripting::Internal;


class ModuleThread {
private:
    std::thread worker;

    std::mutex mutex;
    std::condition_variable cv;

    std::queue<std::function<void()>> commands;

    bool running = true;

    void run() {
        while (true) {
            std::function<void()> command;

            {
                std::unique_lock<std::mutex> lock(mutex);

                // SLEEP HERE until a command arrives.
                cv.wait(lock, [&] {
                    return !commands.empty() || !running;
                });

                if (!running && commands.empty())
                    return;

                command = std::move(commands.front());
                commands.pop();
            }

            command();
        }
    }

public:
    ModuleThread() {
        worker = std::thread(&ModuleThread::run, this);
    }

    void send(std::function<void()> command) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            commands.push(std::move(command));
        }

        cv.notify_one();
    }

	template<typename F>
	auto execute(F&& fn) -> std::invoke_result_t<F> {
		using R = std::invoke_result_t<F>;

		auto task = std::make_shared<std::packaged_task<R()>>(
			std::forward<F>(fn)
		);

		auto future = task->get_future();

		send([task] {
			(*task)();
		});

		return future.get();
	}

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            running = false;
        }

        cv.notify_one();

        if (worker.joinable())
            worker.join();
    }

    ~ModuleThread() {
        stop();
    }
};

struct Module {
	std::unique_ptr<ModuleThread> thread;
	void *setup;
	void *loop;
	void *event;
	void *shutdown;
	bool isMin;
};

std::unordered_map<long, Module> mods;

using FuncPointer = void* (*)();
using FuncPointerP = void* (*)(void *);

long Useable::initModuleJIT(const char* path) {
	static long currID = 0;

	bool ret;

	Module pkg;
	pkg.thread = std::make_unique<ModuleThread>();

	const char* ext = std::strrchr(path, '.');
	if (ext && std::strcmp(ext, ".so") == 0) {
		pkg.thread->execute([&] {
			ret = initModuleJITMin(path);
		});
		pkg.isMin = true;
	}
	else {
		pkg.thread->execute([&] {
			ret = initModuleJITMax(path);
		});
		pkg.isMin = false;
	}

	if (!ret) return -1;

	pkg.thread->execute([&] {

		if (pkg.isMin) {
			pkg.setup = getFunctionJITMin("setup");
			pkg.loop = getFunctionJITMin("loop");
			pkg.event = getFunctionJITMin("event");
			pkg.shutdown = getFunctionJITMin("shutdown");
		}
		else {
			pkg.setup = getFunctionJITMax("setup");
			pkg.loop = getFunctionJITMax("loop");
			pkg.event = getFunctionJITMax("event");
			pkg.shutdown = getFunctionJITMax("shutdown");
		}
	});

	mods[currID] = std::move(pkg);

	return currID++;
}
void Useable::setupJIT(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return;

	void *p = it->second.setup;
	it->second.thread->execute([&]{
		if (p) {
			FuncPointer func = reinterpret_cast<FuncPointer>(p);

			func();
		}
	});
}

void Useable::loopJIT(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return;

	void *p = it->second.loop;
	it->second.thread->execute([&]{
		if (p) {
			FuncPointer func = reinterpret_cast<FuncPointer>(p);

			func();
		}
	});
}

void Useable::eventJIT(long id, void *e) {
	auto it = mods.find(id);
	if (it == mods.end()) return;

	void *p = it->second.event;
	it->second.thread->execute([&]{
		if (p) {
			FuncPointerP func = reinterpret_cast<FuncPointerP>(p);

			func(e);
		}
	});
}

void Useable::shutdownJIT(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return;

	void *p = it->second.shutdown;
	it->second.thread->execute([&]{
		if (p) {
			FuncPointer func = reinterpret_cast<FuncPointer>(p);

			func();
		}
	});
}

void Useable::removeJIT(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return;

	auto &event = it->second;

	it->second.thread->execute([&]{
		if (event.isMin) {
			removeJITMin();
		}
		else {
			removeJITMax();
		}
	});
}

void *Useable::getFunctionJIT(long id, const char *funcname) {
	auto it = mods.find(id);
	if (it == mods.end()) return nullptr;

	void* ret;

	auto &pkg = it->second;

	it->second.thread->execute([&]{
		if (pkg.isMin) {
			ret = getFunctionJITMin(funcname);
		}
		else {
			ret = getFunctionJITMax(funcname);
		}
	});

	return ret;
}

bool Useable::JITRunnable(long id) {

	auto it = mods.find(id);
	if (it == mods.end()) return false;

	auto &mod = it->second;

	return !(mod.loop || mod.event);
}