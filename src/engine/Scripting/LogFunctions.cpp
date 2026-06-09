#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <filesystem>
#include <chrono>
#include <ctime>
#include <string>

std::string makeLogFile()
{
	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);

	std::tm tm{};
	#ifdef _WIN32
	localtime_s(&tm, &t);
	#else
	localtime_r(&t, &tm);
	#endif

	std::filesystem::create_directories("logs");

	return std::format(
		"logs/log_{:04}-{:02}-{:02}_{:02}-{:02}-{:02}.log",
		tm.tm_year + 1900,
		tm.tm_mon + 1,
		tm.tm_mday,
		tm.tm_hour,
		tm.tm_min,
		tm.tm_sec
	);
}

extern "C" void loginit() {
	auto file = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
		makeLogFile(),
		true
	);

	auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

	spdlog::logger logger("", {console, file});
	spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));

	#ifdef DEBUG
	spdlog::set_level(spdlog::level::debug);
	#endif
}