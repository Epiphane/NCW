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

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
