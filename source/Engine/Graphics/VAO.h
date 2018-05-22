// By Thomas Steinke

#pragma once

#include <GLFW/glfw3.h>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

class VAO
{
public:
   VAO() : mVAO(0) {};
   ~VAO();

   void Bind();

protected:
   GLuint mVAO;
};

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
