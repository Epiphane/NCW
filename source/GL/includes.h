// By Thomas Steinke

#pragma once

#include <cassert>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Windows.h includes windef.h includes minwindef.h defines these
#if defined(_WIN32)
#undef near
#undef far
#undef GetObject
#endif

#ifdef NDEBUG
#define CHECK_GL_ERRORS() {}
#else
#define CHECK_GL_ERRORS() { GLenum error = glGetError(); assert(error == 0); }
#endif