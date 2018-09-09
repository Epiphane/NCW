// By Thomas Steinke

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Windows.h includes windef.h includes minwindef.h defines these
#if defined(_WIN32)
   #undef near
   #undef far
#endif