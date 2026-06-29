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

	bool ScriptingShouldStop();
	void ScriptingCreate(const char *path);
	void ScriptingRunLoop();
	void ScriptingRunEvent();
	void ScriptingDestroy();
}

int main(int argc, char *argv[])
{
	loginit();
	Init();
	ScriptingCreate(DEFAULT_MODULE);

	// starts running the game loop
	running = !ScriptingShouldStop();

	while (running)
	{
		Update();
		ScriptingRunEvent();
		ScriptingRunLoop();
	}
	ScriptingDestroy();
	Shutdown();
}

#ifdef _WIN32
// I HATE WINDOWS, THIS IS THE OS FOR THE DEVIL, ONLY THE DEVIL MAKES THEIR OPERATING SYSTEM THIS BAD
#include <windows.h> // EVIL EVIL EVIL, KILL ME
extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
	return main(__argc, __argv);
#endif