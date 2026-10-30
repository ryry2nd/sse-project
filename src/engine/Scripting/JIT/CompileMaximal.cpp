// #define MINIMAL

#include "HeadJITUnusable.hpp"

using namespace Engine::Scripting::Internal;

#ifdef MINIMAL
long Unusable::initModuleJITMax(const char* path) {return -1;}
void Unusable::setupJITMax(long id) {}
bool Unusable::loopJITMax(long id) {return false;}
bool Unusable::eventJITMax(long id, void *e) {return false;}
void Unusable::shutdownJITMax(long id) {}
void Unusable::removeJITMax(long id) {}
bool Unusable::JITRunnableMax(long id) {return false;}
void *Unusable::getFunctionJITMax(long id, const char *funcname) {return nullptr;}
#else
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/Error.h>
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

		auto jtmbOrErr = JITTargetMachineBuilder::detectHost();
		if (!jtmbOrErr) {
			logAllUnhandledErrors(jtmbOrErr.takeError(), errs(), "[JIT] detectHost failed: ");
			return;
		}
		auto jtmb = std::move(*jtmbOrErr);
		jtmb.setRelocationModel(Reloc::PIC_);
		jtmb.setCodeModel(CodeModel::Small); // if you still get "out of range", try CodeModel::Large

		auto jitOrErr = LLJITBuilder()
			.setJITTargetMachineBuilder(std::move(jtmb))
			.create();

		if (!jitOrErr) {
			logAllUnhandledErrors(
				jitOrErr.takeError(), errs(),
				"[JIT] create failed: "
			);
			return;
		}

		m_jit = std::move(*jitOrErr);

		auto gen = DynamicLibrarySearchGenerator::GetForCurrentProcess(
			m_jit->getDataLayout().getGlobalPrefix());
		if (!gen) {
			logAllUnhandledErrors(gen.takeError(), errs(), "[JIT] process symbol generator failed: ");
			return;
		}
		m_jit->getMainJITDylib().addGenerator(std::move(*gen));
	}

	~LLVMRuntime() {
		if (!m_jit)
			return;

		m_initialized = false;
		m_jit.release(); //ToDo make it actually clear the memory
	}

	bool loadModule(const std::string& path) {
		if (!m_jit) {
			errs() << "[JIT] JIT was not created, see earlier error\n";
			return false;
		}

		if (!(path.ends_with(".bc") || path.ends_with(".ll"))) {
			errs() << "[JIT] Unsupported module type: " << path << "\n";
			return false;
		}

		auto context = std::make_unique<LLVMContext>();

		SMDiagnostic err;
		auto module = parseIRFile(path, err, *context);
		if (!module) {
			err.print("[JIT] LLVM Loader", errs());
			return false;
		}

		// Give the module its own __dso_handle so it lands inside the JIT allocation
		if (auto *gv = module->getNamedGlobal("__dso_handle")) {
			if (gv->isDeclaration()) {
				gv->setInitializer(Constant::getNullValue(gv->getValueType()));
				gv->setLinkage(GlobalValue::InternalLinkage);
				gv->setConstant(false);
			}
		}

		for (auto &f : *module)
			if (!f.isDeclaration())
				m_defined.insert(f.getName().str());

		ThreadSafeModule tsm(std::move(module), std::move(context));

		if (auto e = m_jit->addIRModule(std::move(tsm))) {
			logAllUnhandledErrors(std::move(e), errs(), "[JIT] addIRModule failed: ");
			return false;
		}

		if (auto e = m_jit->initialize(m_jit->getMainJITDylib())) {
			logAllUnhandledErrors(std::move(e), errs(), "[JIT] initialize (global constructors) failed: ");
			return false;
		}
		m_initialized = true;

		return true;
	}

	void *getFunction(const std::string& name) {
		auto symbol = m_jit->lookup(name);
		if (!symbol) {
			logAllUnhandledErrors(symbol.takeError(), errs());
			return nullptr;
		}
		return symbol->toPtr<void *>();
	}

private:
	std::unique_ptr<LLJIT> m_jit;
	std::set<std::string> m_defined;
	llvm::orc::LocalCXXRuntimeOverrides m_cxxRuntimeOverrides;
	bool m_initialized = false;
};

struct MyModule {
	std::unique_ptr<LLVMRuntime> runtime;
	void (*setup)() = nullptr;
	void (*loop)() = nullptr;
	void (*event)(void*) = nullptr;
	void (*shutdown)() = nullptr;
};

std::unordered_map<long, MyModule> modsMax;

long Unusable::initModuleJITMax(const char* path) {
	static long currID = 0;

	auto runtime = std::make_unique<LLVMRuntime>();

	if (!runtime->loadModule(path)) {
		return -1;
	}

	MyModule mod;
	mod.setup    = (void (*)())       runtime->getFunction("setup");
	mod.loop     = (void (*)())       runtime->getFunction("loop");
	mod.event    = (void (*)(void *)) runtime->getFunction("event");
	mod.shutdown = (void (*)())       runtime->getFunction("shutdown");
	mod.runtime  = std::move(runtime);

	modsMax.emplace(currID, std::move(mod));
	return currID++;
}

void Unusable::setupJITMax(long id) {
	auto it = modsMax.find(id);
	if (it == modsMax.end() || !it->second.setup) return;
	it->second.setup();
}
bool Unusable::loopJITMax(long id) {
	auto it = modsMax.find(id);
	if (it == modsMax.end() || !it->second.loop) return 1;
	it->second.loop();
	return 0;
}
bool Unusable::eventJITMax(long id, void *e) {
	auto it = modsMax.find(id);
	if (it == modsMax.end() || !it->second.event) return 1;
	it->second.event(e);
	return 0;
}
void Unusable::shutdownJITMax(long id) {
	auto it = modsMax.find(id);
	if (it == modsMax.end() || !it->second.shutdown) return;
	it->second.shutdown();
}
void Unusable::removeJITMax(long id) {
	auto it = modsMax.find(id);
	if (it == modsMax.end()) return;
	it->second.setup = nullptr;
	it->second.loop = nullptr;
	it->second.event = nullptr;
	it->second.shutdown = nullptr;
	modsMax.erase(it);
}
bool Unusable::JITRunnableMax(long id) {
	auto it = modsMax.find(id);
	if (it == modsMax.end()) return 1;

	return !(modsMax[id].loop || modsMax[id].event);
}

void *Unusable::getFunctionJITMax(long id, const char *funcname) {
	auto it = modsMax.find(id);
	if (it == modsMax.end() || !it->second.runtime) return nullptr;
	return it->second.runtime->getFunction(funcname);
}
#endif