#include <engine/Engine.hpp>

extern "C" {
	void setup() {
		Engine::Logging::debug("{} setup successfully", Engine::Module::getCurrent());
	}
}