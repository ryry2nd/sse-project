#include <SDL3/SDL_events.h>
#include <memory>
#include <string>
#include <unordered_map>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>


using namespace llvm;
using namespace llvm::orc;

class LLVMRuntime {
public:
	LLVMRuntime() {
		InitializeNativeTarget();
		InitializeNativeTargetAsmPrinter();
		InitializeNativeTargetAsmParser();

		m_jit = cantFail(LLJITBuilder().create());

		auto &jd = m_jit->getMainJITDylib();

		jd.addGenerator(
			cantFail(DynamicLibrarySearchGenerator::GetForCurrentProcess(
				m_jit->getDataLayout().getGlobalPrefix()
			))
		);
	}

	bool loadModule(const std::string& path) {
		auto context = std::make_unique<LLVMContext>();

		// ---------- IR (.bc / .ll) ----------
		if (path.ends_with(".bc") || path.ends_with(".ll")) {

			SMDiagnostic err;

			auto module = parseIRFile(path, err, *context);
			if (!module) {
				err.print("LLVM Loader", errs());
				return false;
			}

			ThreadSafeModule tsm(
				std::move(module),
				std::move(context)
			);

			if (auto e = m_jit->addIRModule(std::move(tsm))) {
				logAllUnhandledErrors(std::move(e), errs(), "Add IR Module Error: ");
				return false;
			}

			return true;
		}

		errs() << "Unsupported module type: " << path << "\n";
		return false;
	}

	template<typename T>
	T getFunction(const std::string& name) {
		auto symbol = m_jit->lookup(name);

		if (!symbol) {
			return nullptr;
		}

		return symbol->toPtr<T>();
	}

	void reset() {
		m_jit.reset();
	}

private:
	std::unique_ptr<LLJIT> m_jit;
};

struct MyModule {
	std::unique_ptr<LLVMRuntime> runtime;
	void (*setup)();
	void (*loop)();
	void (*event)(SDL_Event*, bool*);
	void (*shutdown)();
};

std::unordered_map<long, MyModule> mods;

long initModule(const char* path) {
	static long currID = 0;

	auto runtime = std::make_unique<LLVMRuntime>();

	if (!runtime->loadModule(path)) {
		return -1;
	}

	auto setup = runtime->getFunction<void(*)()>("setup");
	auto loop = runtime->getFunction<void(*)()>("loop");
	auto event = runtime->getFunction<void(*)(SDL_Event *, bool *)>("event");
	auto shutdown = runtime->getFunction<void(*)()>("shutdown");

	MyModule mod;

	mod.runtime = std::move(runtime);
	mod.setup = setup;
	mod.loop = loop;
	mod.event = event;
	mod.shutdown = shutdown;

	mods[currID] = std::move(mod);
	return currID++;
}

void setup(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return;

	if (!mods[id].setup) return;

	mods[id].setup();
}
bool loop(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return 1;

	if (!mods[id].loop)	return 1;

	mods[id].loop();
	return 0;
}
bool event(long id, SDL_Event *e, bool *running) {
	auto it = mods.find(id);
	if (it == mods.end()) return 1;

	if (!mods[id].event) {
		return 1;
	}
	mods[id].event(e, running);
	return 0;
}
void shutdown(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return;

	if (!mods[id].shutdown) {
		return;
	}
	mods[id].shutdown();
}
void remove(long id) {
	auto it = mods.find(id);
	if (it == mods.end()) return;

	mods[id].setup = nullptr;
	mods[id].loop = nullptr;
	mods[id].event = nullptr;
	mods[id].shutdown = nullptr;

	mods[id].runtime.reset();
	mods.erase(id);
}