#include <spdlog/spdlog.h>

#include "HeadJIT.hpp"

extern "C" {
	void hostLogInfo(const char* str) {
		spdlog::info("{}", str);
	}
	void hostLogWarn(const char* str) {
		spdlog::warn("{}", str);
	}
	void hostLogDebug(const char* str) {
		spdlog::debug("{}", str);
	}
	void hostLogError(const char* str) {
		spdlog::error("{}", str);
	}
	void hostLogCritical(const char* str) {
		spdlog::critical("{}", str);
	}
	void engineShutdown() {}

	void *hostGetFunction(const char *libname, const char *funcname) {
		using namespace Engine::Scripting::Internal::Useable;
		auto func = getFunctionJIT(idTollvm(libname), funcname);
		return func;
	}

	const char *hostIdToPath(const char *id) {
		using namespace Engine::Scripting::Internal::Useable;
		auto ret = idToPath(id);
		return ret;
	}
}