#pragma once

#include <engine/Engine.hpp>

#include <StaticIDs.hpp>

inline void foo() {
	((void (*)()) Engine::Module::getFunction(stdmod::get_MODULE_ID(), "foo"))();
}