#include <engine/Engine.hpp>

#include <StaticIDs.hpp>

static const char *MODULE_PATH;
static const char *MODULE_ID;

const char *stdmod::get_MODULE_PATH() {
	return MODULE_PATH;
}
const char *stdmod::get_MODULE_ID() {
	return MODULE_ID;
}

extern "C" {
	void setup() {
		MODULE_ID = Engine::Module::getCurrentRunning();
		MODULE_PATH = Engine::Module::idToPath(MODULE_ID);
	}
}