#include <engine/Scripting/ScriptingHeaders.hpp>
#include <SDL3/SDL_events.h>

#ifndef DEFAULT_MODULE
#define DEFAULT_MODULE "modules/example1"
#endif

static bool running = true;

extern "C" {
	void engineShutdown() {
		running = false;
	}
	void Init();
	void Update();
	void Shutdown();
	void loginit();
}

int main(int argc, char *argv[])
{
	loginit();
	Init();
	ScriptingHeaders::EnginePackage *example_package = new ScriptingHeaders::EnginePackage(std::string(DEFAULT_MODULE));

	// starts running the game loop
	running = !ScriptingHeaders::EnginePackage::ShouldStop();

	while (running)
	{
		Update();
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			example_package->EventFunction(&e);
		}
		example_package->LoopFunction();
	}

	delete example_package;
	Shutdown();
}

#ifdef _WIN32
// I HATE WINDOWS, THIS IS THE OS FOR THE DEVIL, ONLY THE DEVIL MAKES THEIR OPERATING SYSTEM THIS BAD
#include <windows.h> // EVIL EVIL EVIL, KILL ME
extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
	return main(__argc, __argv);
#endif