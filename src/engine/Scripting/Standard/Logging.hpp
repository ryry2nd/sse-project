#pragma once

#include <format>

extern "C" void hostLogInfo(const char* str);
extern "C" void hostLogWarn(const char* str);
extern "C" void hostLogDebug(const char* str);
extern "C" void hostLogError(const char* str);
extern "C" void hostLogCritical(const char* str);

namespace Engine::Logging {

template<typename... Args>
inline void info(std::format_string<Args...> fmtStr, Args&&... args) {
	auto msg = std::format(fmtStr, std::forward<Args>(args)...);
	hostLogInfo(msg.c_str());
}

template<typename... Args>
inline void warn(std::format_string<Args...> fmtStr, Args&&... args) {
	auto msg = std::format(fmtStr, std::forward<Args>(args)...);
	hostLogWarn(msg.c_str());
}

template<typename... Args>
inline void debug(std::format_string<Args...> fmtStr, Args&&... args) {
	auto msg = std::format(fmtStr, std::forward<Args>(args)...);
	hostLogDebug(msg.c_str());
}

template<typename... Args>
inline void error(std::format_string<Args...> fmtStr, Args&&... args) {
	auto msg = std::format(fmtStr, std::forward<Args>(args)...);
	hostLogError(msg.c_str());
}

template<typename... Args>
inline void critical(std::format_string<Args...> fmtStr, Args&&... args) {
	auto msg = std::format(fmtStr, std::forward<Args>(args)...);
	hostLogCritical(msg.c_str());
}

}