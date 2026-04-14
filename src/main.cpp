#include "engine/objects/Objects.hpp"
#include "engine/rendering/base/Rendering.hpp"
#include "engine/scripting/ScriptingHeaders.hpp"

int main(int argc, char *argv[])
{    
    ScriptingHeaders::Package *example_package = new ScriptingHeaders::Package("modules/example");

    // starts running the game loop
    bool running = true;

    while (running)
    {
        Rendering::Window::Update();

        ScriptingHeaders::Package::EventFunctions(&running);
        ScriptingHeaders::Package::LoopFunctions();

        Objects::Drawable::UpdateAllObjects();

        for (auto &win : Rendering::sdlWindows) {
            win->clearBackground();
        }

        Objects::Drawable::DrawAllObjects();

        for (auto &win : Rendering::sdlWindows) {
            win->swapBuffer();
        }
    }
    
    delete example_package;
    return 0;
}

#ifdef _WIN32 // I HATE WINDOWS, THIS IS THE OS FOR THE DEVIL, ONLY THE DEVIL MAKES THEIR OPERATING SYSTEM THIS BAD

#include <windows.h> // EVIL EVIL EVIL, KILL ME

extern "C" int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}
#endif