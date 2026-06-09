#pragma once
#include <string>

union SDL_Event;

namespace ScriptingHeaders
{
	class EnginePackage
	{
	public:
		std::string getName() {return name;}
		std::string getID() {return id;}
		std::string getVersion() {return version;}
		std::string getPath() {return path;}

		void LoopFunction();
		void EventFunction(SDL_Event *event);
		static bool ShouldStop();

		EnginePackage(const std::string &path);
		~EnginePackage();

	private:
		std::string name;
		std::string id;
		std::string version;
		std::string path;

		long llvmLocation;
	};
}