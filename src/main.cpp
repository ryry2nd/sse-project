#include <engine/Rendering/Rendering.hpp>
#include <engine/Scripting/ScriptingHeaders.hpp>

#define DEFAULT_MODULE "modules/example1"

int main(int argc, char *argv[])
{
	ScriptingHeaders::loginit();
	Rendering::Window::init();
	ScriptingHeaders::EnginePackage *example_package = new ScriptingHeaders::EnginePackage(std::string(DEFAULT_MODULE));

	// starts running the game loop
	bool *running = new bool(true);

	while (*running)
	{
		Rendering::Window::Update();

		ScriptingHeaders::EnginePackage::EventFunctions(running);

		// for (auto &win : Rendering::sdlWindows) {
		// 	win->clearBackground();
		// }

		ScriptingHeaders::EnginePackage::LoopFunctions();

		// for (auto &win : Rendering::sdlWindows) {
		// 	win->swapBuffer();
		// }
		if (ScriptingHeaders::EnginePackage::ShouldStop()) *running = false;
	}

	delete example_package;
	Rendering::Window::shutdown();
	delete running;
	return 0;
}

#ifdef _WIN32 // I HATE WINDOWS, THIS IS THE OS FOR THE DEVIL, ONLY THE DEVIL MAKES THEIR OPERATING SYSTEM THIS BAD

#include <windows.h> // EVIL EVIL EVIL, KILL ME

extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return main(__argc, __argv);
}
#endif