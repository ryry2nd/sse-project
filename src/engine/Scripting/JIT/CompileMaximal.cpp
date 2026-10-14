#define MINIMAL

#include "HeadJITUnusable.hpp"

using namespace Engine::Scripting::Internal;

long Unusable::initModuleJITMax(const char* path) {return -1;}
void Unusable::setupJITMax(long id) {}
bool Unusable::loopJITMax(long id) {return false;}
bool Unusable::eventJITMax(long id, void *e) {return false;}
void Unusable::shutdownJITMax(long id) {}
void Unusable::removeJITMax(long id) {}
bool Unusable::JITRunnableMax(long id) {return false;}
void *Unusable::getFunctionJITMax(long id, const char *funcname) {return nullptr;}

#ifndef MINIMAL
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

	~LLVMRuntime() {
		if (m_jit) {
			m_jit.reset();
		}
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

	void (*getFunction(const std::string& name))() {
		auto symbol = m_jit->lookup(name);

		if (!symbol) {
			logAllUnhandledErrors(symbol.takeError(), errs());
			return nullptr;
		}

		return symbol->toPtr<void (*)()>;
	}

private:
	std::unique_ptr<LLJIT> m_jit;
};

struct MyModule {
	LLVMRuntime *runtime;
	void (*setup)();
	void (*loop)();
	void (*event)(SDL_Event*);
	void (*shutdown)();
};

std::unordered_map<long, MyModule> modsMax;

long initModuleJITMax(const char* path) {
	static long currID = 0;

	auto runtime = new LLVMRuntime();

	if (!runtime->loadModule(path)) {
		return -1;
	}

	auto setup = runtime->getFunction("setup");
	auto loop = runtime->getFunction("loop");
	auto event = runtime->getFunction("event");
	auto shutdown = runtime->getFunction("shutdown");

	MyModule mod;

	mod.runtime = runtime;
	mod.setup = setup;
	mod.loop = loop;
	mod.event = (void (*)(SDL_Event *)) event;
	mod.shutdown = shutdown;

	modsMax[currID] = std::move(mod);
	return currID++;
}

void Unusable::setupJITMax(long id) {
	auto it = modsMax.find(id);
	if (it == modsMax.end()) return;

	if (!modsMax[id].setup) return;

	modsMax[id].setup();
}
bool Unusable::loopJITMax(long id) {
	auto it = modsMax.find(id);
	if (it == modsMax.end()) return 1;

	if (!modsMax[id].loop)	return 1;

	modsMax[id].loop();
	return 0;
}
bool Unusable::eventJITMax(long id, SDL_Event *e) {
	auto it = modsMax.find(id);
	if (it == modsMax.end()) return 1;

	if (!modsMax[id].event) {
		return 1;
	}
	modsMax[id].event(e);
	return 0;
}
void Unusable::shutdownJITMax(long id) {
	auto it = modsMax.find(id);
	if (it == modsMax.end()) return;

	if (!modsMax[id].shutdown) {
		return;
	}
	modsMax[id].shutdown();
}
void Unusable::removeJITMax(long id) {
	auto it = modsMax.find(id);
	if (it == modsMax.end()) return;

	auto &mod = modsMax[id];

	mod.setup = nullptr;
	mod.loop = nullptr;
	mod.event = nullptr;
	mod.shutdown = nullptr;

	delete mod.runtime;

	modsMax.erase(id);
}
bool Unusable::JITRunnableMax(long id) {
	auto it = modsMax.find(id);
	if (it == modsMax.end()) return 0;

	return !(modsMax[id].loop || modsMax[id].event);
}

void *Unusable::getFunctionJITMax(long id, const char *funcname) {
	return (void *) modsMax[id].runtime->getFunction(funcname);
}
#endif