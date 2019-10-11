//
//  main.h
//  FinalProject
//
//  Created by Thomas Steinke on 3/3/15.
//  Copyright (c) 2015 Thomas Steinke. All rights reserved.
//

#pragma once

#include <vector>
#pragma warning(disable : 4201)
#include <glm/glm.hpp>
#pragma warning(default : 4201)

#include "../GLSL.h"

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

#define INIT_BENCHMARK float _clock = glfwGetTime();
#define COMPUTE_BENCHMARK(samp, msg, everyframe) {\
   static float _samples[samp] = {1};\
   static int _pos = 0;\
   _samples[_pos] = glfwGetTime() - _clock;\
   _pos = (_pos + 1) % samp;\
   float _elapsed = 0;\
   for (int i = 0; i < samp; i ++)\
      _elapsed += _samples[i];\
   _elapsed = _elapsed / samp;\
   /*RendererDebug::Instance().log(msg + std::to_string(_elapsed), !everyframe);*/\
   _clock = glfwGetTime(); /* Chain debugging */ \
   }

extern int w_width;
extern int w_height;
extern float aspect_ratio;
extern bool fullscreen;
