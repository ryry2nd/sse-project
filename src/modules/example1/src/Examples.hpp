#pragma once

#include <string>
#include <engine/Engine.hpp>
#include <glm/glm.hpp>

inline std::string MODULE_PATH;
inline std::string MODULE_ID;

inline Engine::Rendering::Window *win = nullptr;
inline Engine::Rendering::FrameBuffer *fbo = nullptr;
inline Engine::Rendering::Font *font = nullptr;

inline constexpr glm::vec2 res(900.0f, 500.0f);
inline constexpr float ratio = res.x / res.y;

enum Mode : Uint32
{
	Men		= 0,
	Ex1		= 1,
	Ex2		= 2,
};

inline Uint32 mode = Mode::Ex1;


namespace Menu {
	void setup();
	void loop();
	void event(Engine::Input::Event *);
	void shutdown();
}

namespace Example1 {
    void setup();
    void loop();
    void event(Engine::Input::Event *);
	void shutdown();
}

namespace Example2 {
    void setup();
    void loop();
    void event(Engine::Input::Event *);
	void shutdown();
}