#include <spdlog/spdlog.h>

extern "C" void hostLogInfo(const char* str) {
    spdlog::info("{}", str);
}
extern "C" void hostLogWarn(const char* str) {
    spdlog::warn("{}", str);
}
extern "C" void hostLogDebug(const char* str) {
    spdlog::debug("{}", str);
}
extern "C" void hostLogError(const char* str) {
    spdlog::error("{}", str);
}
extern "C" void hostLogCritical(const char* str) {
    spdlog::critical("{}", str);
}