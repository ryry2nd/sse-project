#include <engine/Engine.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <SDL3/SDL_events.h>

#include <Meshes.hpp>
#include <MathTest.hpp>
#include <FooTest.hpp>
#include <Object.hpp>

#include "Examples.hpp"

using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Helper;
using namespace Engine::Logging;
using namespace Engine::Input;

using namespace ObjectSpace;

Object instructions;

void Menu::setup() {
	win->setMode(Window::WindowMode::WindowedMode);
	win->setBackgroundColor({0, 0, 0, 1.0f});

	auto build2DFace = (void (*) (
		std::vector<Vert>& vertices,
		std::vector<Ind>& indices,
		glm::vec2 size,
		glm::vec3 position,
		glm::vec2 uvMin,
		glm::vec2 uvMax
	)) Module::getFunction("com.ryan.std", "build2DFace");

	Surface texsurf;
	font->RenderText("Type the example you want", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &texsurf);
	Image *fontimg = createImage(&texsurf, ImageSettings::tex());

	Shader *tex2dShader = createShader((MODULE_PATH + "/shaders/tex2d").c_str());

	std::vector<Vert> vertsfbo;
	std::vector<Ind> indsfbo;

	build2DFace(vertsfbo, indsfbo, {1, 1}, {0, 0, 1}, {0, 0}, {1, 1});

	Mesh *flatMesh = createMesh(vertsfbo.data(), vertsfbo.size(), indsfbo.data(), indsfbo.size());

	glm::mat4 fpsMpv = fboMPV(res, {0, res.y - 100, 0.5}, {res.x, 100});
	Buff *fpsBuff = createBuff(Rendering::Buff::Type::Uniform, Rendering::Buff::Frequency::Dynamic, sizeof(glm::mat4), &fpsMpv);
	instructions.mat[0].shader = tex2dShader;
	instructions.mat[0].ubo[0] = fpsBuff;
	instructions.mat[0].images[0] = fontimg;
	instructions.mat[0].settings |= Material::EnableClear;
	instructions.mesh[0] = flatMesh;


}

void Menu::loop() {
	win->update();
	win->clearBackground();

	instructions.draw(win);

	win->swapBuffer();
}

void Menu::event(Event *event) {
	auto type = event->type;
	auto dt = win->getDeltaTime();
	if (type == EventType::KeyDown)
	{
		auto key = event->key;
		if (key == SDLK_ESCAPE) {
			Engine::shutdown();
			return;
		}
		else if (key == SDLK_1) {
			Menu::shutdown();
			Example1::setup();
			mode = Mode::Ex1;
			return;
		}
	}
}

void Menu::shutdown() {
	instructions.clear();
}