#include <MathTest.hpp>

#include <engine/Engine.hpp>

int MathTest::add(int x, int y) {return x + y;}
int MathTest::sub(int x, int y) {return x - y;}
int MathTest::mul(int x, int y) {return x * y;}
int MathTest::div(int x, int y) {return x / y;}

#include <StaticIDs.hpp>

extern "C" void foo() {
	Engine::Logging::debug("Hello From, {}", stdmod::get_MODULE_ID());
}