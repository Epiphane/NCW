// By Thomas Steinke

#include <glad/glad.h>

#include "VAO.h"

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

VAO::~VAO()
{
   if (mVAO != 0)
   {
      glDeleteVertexArrays(1, &mVAO);
   }
}

void VAO::Bind()
{
   if (mVAO == 0)
   {
      glGenVertexArrays(1, &mVAO);
   }
   glBindVertexArray(mVAO);
}


}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld