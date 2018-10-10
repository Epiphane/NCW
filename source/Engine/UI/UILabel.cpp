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
   
}; // namespace Engine

}; // namespace CubeWorld
