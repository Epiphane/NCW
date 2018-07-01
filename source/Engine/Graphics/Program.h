// By Thomas Steinke

#pragma once

#include <GLFW/glfw3.h>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

typedef GLuint Program;

Program LoadProgram(
   const char* vertexShaderPath,
   const char* geometryShaderPath,
   const char* fragmentShaderPath
);

inline Program LoadProgram(
   const char* vertexShaderPath,
   const char* fragmentShaderPath
) {
   return LoadProgram(vertexShaderPath, nullptr, fragmentShaderPath);
}

// Helper functions for render systems.
#define REGISTER_GLUINT(RENDERER, value) GLuint RENDERER::value = 0;
#define DISCOVER_UNIFORM(value) {value = glGetUniformLocation(program, #value); GLenum error = glGetError(); assert(error == 0); }
#define DISCOVER_ATTRIBUTE(value) {value = glGetAttribLocation(program, #value); GLenum error = glGetError(); assert(error == 0); }

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
