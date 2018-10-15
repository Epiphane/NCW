// By Thomas Steinke

#include <Engine/Graphics/Program.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/Helpers/Asset.h>

#include "UILabel.h"

namespace CubeWorld
{

namespace Engine
{

UILabel::UILabel(UIRoot* root, UIElement* parent) 
   : UIElement(root, parent)
   , mText("")
   , mRegion(root->Reserve<Engine::Aggregator::Text>(30))
{
   auto maybeFont = Engine::Graphics::FontManager::Instance()->GetFont(Asset::Font("debug"));
   assert(maybeFont);
   mpFont = *maybeFont;
   
   root->GetAggregator<Engine::Aggregator::Text>()->ConnectToTexture(mRegion, mpFont->GetTexture());
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
   
void UILabel::Receive(const Engine::UIRebalancedEvent& evt)
{
   RenderText();
}
   
/**
 * Updates the Aggregator with the vertices corresponding to my current text and position 
 */
void UILabel::RenderText()
{
   std::vector<Engine::Graphics::Font::CharacterVertexUV> uvs = mpFont->Write(mFrame.left.int_value(), mFrame.bottom.int_value(), 1, mText);
   std::vector<Engine::Aggregator::TextData> data;
   
   std::transform(uvs.begin(), uvs.end(), std::back_inserter(data), [](const Engine::Graphics::Font::CharacterVertexUV& character) {
      return Engine::Aggregator::TextData{character.position, character.uv};
   });
   
   while (data.size() < mRegion.size())
   {
      data.push_back(Engine::Aggregator::TextData{glm::vec2(0),glm::vec2(0)});
   }
   
   mRegion.Set(data.data());
}
   
}; // namespace Engine

}; // namespace CubeWorld
