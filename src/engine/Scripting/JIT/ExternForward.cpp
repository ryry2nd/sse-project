#include <spdlog/spdlog.h>

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
}