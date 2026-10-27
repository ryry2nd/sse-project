#pragma once

#ifdef DEBUG
#include <tracy/Tracy.hpp>
#define SCOPE(name) ZoneScopedN(name);
#define FRAMEMARK FrameMark;
#else
#define SCOPE(name)
#define FRAMEMARK
#endif