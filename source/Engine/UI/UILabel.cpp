// By Thomas Steinke

#include <Engine/Graphics/Program.h>
#include <Shared/Helpers/Asset.h>

#include "UILabel.h"

namespace CubeWorld
{

namespace Engine
{

UILabel::UILabel(UIRoot* root, UIElement* parent) : UIElement(root, parent)
{
   auto maybeFont = Engine::Graphics::FontManager::Instance()->GetFont(Asset::Font("debug"));
   assert(maybeFont);
   mpFont = *maybeFont;
      
   mInnateHorizontalSize = (mFrame.width  == 0);
   mInnateVerticalSize =   (mFrame.height == 0);
}

/** Set the label's text. Will change the label's innate size. */
void UILabel::SetText(const std::string& text) {
   mText = text;
}

/** Get this label's text. */
std::string UILabel::GetText() {
   return mText;
}

/** Set the color for the text */
void UILabel::SetTextColor(float r, float g, float b, float a) {
   mTextColor = glm::vec4(r, g, b, a);
}

/** Get the color for the text */
glm::vec4 UILabel::GetTextColor() {
   return mTextColor;
}

/**
   * Add the label's vertices to the root's list of vertices.
   */
void UILabel::AddVertices(std::vector<Graphics::Font::CharacterVertexUV>& outVertices) {
   std::vector<Graphics::Font::CharacterVertexUV> chars = mpFont->Write(0, 0, 1, mText);
   outVertices.insert(outVertices.end(), chars.begin(), chars.end());
      
   // Calculate my new built-in size
      
   Graphics::Font::CharacterVertexUV topLeft, bottomRight;
   topLeft    .position = glm::vec2(mFrame.left.int_value(),  mFrame.top.int_value());
   bottomRight.position = glm::vec2(mFrame.right.int_value(), mFrame.bottom.int_value());
      
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
size_t UILabel::Render(Engine::Graphics::VBO& /*vbo*/, size_t offset) {
//      Window* pWindow = Window::Instance();
//      
//      glUseProgram(program);
//      
//      glUniform4f(uColor, mTextColor.r, mTextColor.g, mTextColor.b, mTextColor.a);
//      glUniform2f(uWindowSize, static_cast<GLfloat>(pWindow->GetWidth()), static_cast<GLfloat>(pWindow->GetHeight()));
//      
//      vbo.AttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)offset);
//      
//      glDrawArrays(GL_LINES, 0, 2);
//      
//      // Cleanup.
//      glDisableVertexAttribArray(aPosition);
//      glUseProgram(0);
//      
//      offset = UIElement::Render(vbo, offset + sizeof(glm::vec2) * 2);
//      
//      return offset;
   return offset;
}
   
}; // namespace Engine

}; // namespace CubeWorld
