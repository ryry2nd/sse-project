#pragma once
#include <string>
#include <vector>
#include <spdlog/spdlog.h>

struct SDL_SharedObject;
union SDL_Event;

namespace ScriptingHeaders
{
	class EnginePackage
	{
	public:
		std::string name;
		std::string id;
		std::string version;
		std::string path;

		static std::vector<EnginePackage *> packages;
		static void LoopFunctions();
		static void EventFunctions(bool *running);
		static bool ShouldStop();

		EnginePackage(const std::string &path);
		~EnginePackage();

	private:
		long llvmLocation;

		void runLoopFunction();
		void runEventFunction(SDL_Event *event, bool *running);
	};

	void loginit();
}