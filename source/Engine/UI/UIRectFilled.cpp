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
   
   
   /** Sets the fill color of this rectangle */
   void UIRectFilled::SetColor(float r, float g, float b, float a) {
      mColor = glm::vec4(r, g, b, a);
   }
   
   
   /** Returns the fill color of this rectangle */
   glm::vec4 UIRectFilled::GetColor() {
      return mColor;
   }
   
   
   void UIRectFilled::AddVertices(std::vector<Graphics::Font::CharacterVertexUV>& outVertices) {
      Graphics::Font::CharacterVertexUV topLeft, bottomRight;
      topLeft    .position = glm::vec2(mFrame.left.int_value(),  mFrame.top.int_value());
      bottomRight.position = glm::vec2(mFrame.right.int_value(), mFrame.bottom.int_value());
      
      topLeft.uv = glm::vec2(0, 0);
      bottomRight.uv = glm::vec2(0, 0);
      
      outVertices.push_back(topLeft);
      outVertices.push_back(bottomRight);
      
      UIElement::AddVertices(outVertices);
   }
   
   /**
    * Makes the OpenGL calls to render a solid rectangle.
    *
    * `offset` is the current offset into the VBO we're rendering with.
    *    This method returns the NEW offset (in this case, 2 vertices
    *    past the previous offset)
    */
   int UIRectFilled::Render(Engine::Graphics::VBO& vbo, size_t offset) {
      Window* pWindow = Window::Instance();
      
      glUseProgram(program);
      
      glUniform4f(uColor, mColor.r, mColor.g, mColor.b, mColor.a);
      glUniform2f(uWindowSize, static_cast<GLfloat>(pWindow->Width()), static_cast<GLfloat>(pWindow->Height()));
      
      vbo.AttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Graphics::Font::CharacterVertexUV), (void*)offset);
      
      glDrawArrays(GL_LINES, 0, 2);
      
      // Cleanup.
      glDisableVertexAttribArray(aPosition);
      glUseProgram(0);
      
      offset = UIElement::Render(vbo, offset + sizeof(Graphics::Font::CharacterVertexUV) * 2);
      
      return offset;
   }
    
   
}; // namespace Engine

}; // namespace CubeWorld
