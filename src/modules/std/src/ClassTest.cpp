#include <ClassTest.hpp>

int ClassTest::foo(int x, int y) {
	return x << y;
}

extern "C" ClassTest *createClassTest() {
	return new ClassTest();
}