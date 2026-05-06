#pragma once
#include <string>
#include <vector>
#include <spdlog/spdlog.h>

struct SDL_SharedObject;
union SDL_Event;

namespace ScriptingHeaders
{
	typedef void(*FuncType)();
	typedef void(*EventType)(SDL_Event*, bool*);
	class Package
	{
	public:
		std::string name;
		std::string id;
		std::string version;
		std::string path;

		static std::vector<Package *> packages;
		static void LoopFunctions();
		static void EventFunctions(bool *running);

		Package(const std::string &path);
		~Package();

	private:
		SDL_SharedObject* lib = nullptr;
		FuncType loopFunc = nullptr;
		EventType eventFunc = nullptr;

		void runLoopFunction();
		void runEventFunction(SDL_Event *event, bool *running);
	};

	void loginit();
}