// By Thomas Steinke

#pragma once

namespace CubeWorld
{

namespace Engine
{

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef MATH_PI
#define MATH_PI M_PI
#endif

#define RADIANS_TO_DEG float(180.0f / M_PI)
#define DEG_TO_RADIANS float(M_PI / 180.0f)
#define DEGREES(radians) (radians * RADIANS_TO_DEG)
#define RADIANS(degrees) (degrees * DEG_TO_RADIANS)

#ifndef TIMEDELTA
#define TIMEDELTA double
#endif

}; // namespace Engine

}; // namespace CubeWorld
