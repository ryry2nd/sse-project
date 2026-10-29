#include <SDL3/SDL_scancode.h>
#include "Core.hpp"

namespace Engine::Input {
	enum class EventType : Uint8
    {
        None,
        Quit,
        KeyDown,
        KeyUp,
        MouseMove,
        MouseButtonDown,
        MouseButtonUp,
        MouseWheel,
        WindowResize,
        WindowClose
    };

    struct Event
    {
        EventType type = EventType::None;

        struct
        {
            int x = 0;
            int y = 0;
			int xrel = 0;
            int yrel = 0;
        } mouse;

        struct
        {
            int width = 0;
            int height = 0;
        } window;

        Uint32 key = 0;
        Uint8 button = 0;
        bool repeat = false;
    };
}