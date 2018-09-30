// By Thomas Steinke

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <Engine/Core/Window.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>

#include "UIRectFilled.h"

namespace CubeWorld
{

namespace Engine
{
   
   REGISTER_GLUINT(UIRectFilled, program);
   
   REGISTER_GLUINT(UIRectFilled, aPosition);
   REGISTER_GLUINT(UIRectFilled, uColor);
   REGISTER_GLUINT(UIRectFilled, uWindowSize);

   
   UIRectFilled::UIRectFilled() {
      if (program == 0) {
         program = Engine::Graphics::LoadProgram("Shaders/DebugRect.vert", "Shaders/DebugRect.geom", "Shaders/DebugRect.frag");
      }
      
      if (program == 0)
      {
         LOG_ERROR("Could not load DebugRect shader");
         return;
      }
      
      DISCOVER_ATTRIBUTE(aPosition);
      DISCOVER_UNIFORM(uColor);
      DISCOVER_UNIFORM(uWindowSize);
   }
   
   
   void UIRectFilled::AddVertices(std::vector<VertexData>& outVertices) {
      VertexData topLeft, bottomRight;
      topLeft    .position = glm::vec2(mFrame.left.int_value(),  mFrame.top.int_value());
      bottomRight.position = glm::vec2(mFrame.right.int_value(), mFrame.bottom.int_value());
      
//      topLeft    .position = glm::vec2(100, 300);
//      bottomRight.position = glm::vec2(500, 600);
      
      
      outVertices.push_back(topLeft);
      outVertices.push_back(bottomRight);
      
      UIElement::AddVertices(outVertices);
   }
   
   
   void UIRectFilled::Render(Engine::Graphics::VBO& vbo, size_t offset) {
      Window* pWindow = Window::Instance();
      
      glUseProgram(program);
      
      glUniform4f(uColor, 0.3f, 0.3f, 0.4f, 0.6f);
      glUniform2f(uWindowSize, static_cast<GLfloat>(pWindow->Width()), static_cast<GLfloat>(pWindow->Height()));
      
      vbo.AttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)offset);
      
      glDrawArrays(GL_LINES, 0, 2);
      
      // Cleanup.
      glDisableVertexAttribArray(aPosition);
      glUseProgram(0);
      
      UIElement::Render(vbo, offset + 2);
   }
   
   
}; // namespace Engine

}; // namespace CubeWorld
