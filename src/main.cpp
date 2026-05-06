#include <engine/Rendering/Rendering.hpp>
#include <engine/Scripting/ScriptingHeaders.hpp>

#define DEFAULT_MODULE "modules/example1"

int main(int argc, char *argv[])
{
	ScriptingHeaders::loginit();
	Rendering::Window::init();
	ScriptingHeaders::Package *example_package = new ScriptingHeaders::Package(std::string(DEFAULT_MODULE));

	// starts running the game loop
	bool running = true;

	while (running)
	{
		Rendering::Window::Update();

		ScriptingHeaders::Package::EventFunctions(&running);

		for (auto &win : Rendering::sdlWindows) {
			win->clearBackground();
		}

		ScriptingHeaders::Package::LoopFunctions();

		for (auto &win : Rendering::sdlWindows) {
			win->swapBuffer();
		}
	}

	delete example_package;
	Rendering::Window::shutdown();
	return 0;
}

#ifdef _WIN32 // I HATE WINDOWS, THIS IS THE OS FOR THE DEVIL, ONLY THE DEVIL MAKES THEIR OPERATING SYSTEM THIS BAD

#include <windows.h> // EVIL EVIL EVIL, KILL ME

extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return main(__argc, __argv);
}
#endif