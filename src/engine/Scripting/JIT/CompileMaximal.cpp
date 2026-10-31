#include "HeadJITUnusable.hpp"

using namespace Engine::Scripting::Internal;

#ifdef MINIMAL
bool Unusable::initModuleJITMax(const char* path) {return false;}
void Unusable::removeJITMax() {}
void *Unusable::getFunctionJITMax(const char *funcname) {return nullptr;}
#else
#include <memory>
#include <set>
#include <string>

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

std::unique_ptr<LLVMRuntime> runtimeMax = nullptr;

bool Unusable::initModuleJITMax(const char* path) {
	if (runtimeMax)
		runtimeMax.reset();

	runtimeMax = std::make_unique<LLVMRuntime>();

	return runtimeMax->loadModule(path);
}

void Unusable::removeJITMax() {
	if (runtimeMax)
		runtimeMax.reset();

	runtimeMax = nullptr;
}

void *Unusable::getFunctionJITMax(const char *funcname) {
	if (runtimeMax)
		return runtimeMax->getFunction(funcname);
	return nullptr;
}
#endif