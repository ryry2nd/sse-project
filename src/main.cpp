#include "engine/BigObjects/BigObjects.hpp"
#include "engine/Rendering/base/Rendering.hpp"
#include "engine/Scripting/ScriptingHeaders.hpp"

int main(int argc, char *argv[])
{    
    Rendering::Window::init();
    ScriptingHeaders::Package *example_package = new ScriptingHeaders::Package("modules/example1");

    // starts running the game loop
    bool running = true;

    while (running)
    {
        Rendering::Window::Update();

        ScriptingHeaders::Package::EventFunctions(&running);
        ScriptingHeaders::Package::LoopFunctions();

        BigObjects::Drawable::UpdateAllObjects();

        for (auto &win : Rendering::sdlWindows) {
            win->clearBackground();
        }

        BigObjects::Drawable::DrawAllObjects();

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