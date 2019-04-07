// By Thomas Steinke

#include <cassert>

#include <RGBDesignPatterns/Scope.h>
#include <Engine/Core/Window.h>
#include <RGBLogger/Logger.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/Helpers/Asset.h>

#include "Text.h"

namespace CubeWorld
{

namespace UI
{

Text::Text(Engine::UIRoot* root, UIElement* parent, const Options& options, const std::string& name)
   : UIElement(root, parent, name)
   , mText("")
   , mRendered("")
{
   auto maybeFont = Engine::Graphics::FontManager::Instance()->GetFont(Asset::Font(options.font));
   assert(maybeFont);
   mFont = *maybeFont;

   uint32_t size = options.size;
   if (size == 0)
   {
      size = options.DefaultSize();
   }
   mRegion = root->Reserve<Aggregator::Text>(2 * size);

   // Suggest a size based on what we reserved.
   root->AddConstraints({
      rhea::constraint(mFrame.right - mFrame.left >= size * 14.0, rhea::strength::weak()),
      rhea::constraint(mFrame.top - mFrame.bottom >= 28.0, rhea::strength::weak())
   });
   
   mpRoot->AddEditVar(mTextContentWidth);
   mpRoot->AddEditVar(mTextContentHeight);

   SetText(options.text);
   root->GetAggregator<Aggregator::Text>()->ConnectToTexture(mRegion, mFont->GetTexture());
}

void Text::SetText(const std::string& text)
{
   if (text == mText)
   {
      return;
   }

   mText = text;
   RenderText(text);
   RecalculateSize();
}

void Text::SetAlignment(Engine::Graphics::Font::Alignment newAlignment)
{
   mAlignment = newAlignment;
   Redraw();
}

Engine::Graphics::Font::Alignment Text::GetAlignment()
{
   return mAlignment;
}

void Text::Redraw()
{
   std::vector<Aggregator::TextData> data;

   if (mActive)
   {
      std::vector<Engine::Graphics::Font::CharacterVertexUV> uvs = mFont->Write(GLfloat(mFrame.left.value()), GLfloat(mFrame.bottom.value()), GLfloat(mFrame.GetWidth()), 1, mRendered, mAlignment);
      std::transform(uvs.begin(), uvs.end(), std::back_inserter(data), [&](const Engine::Graphics::Font::CharacterVertexUV& character) {
         return Aggregator::TextData{glm::vec3(character.position.x, character.position.y, mFrame.z.value()), character.uv};
      });
   }

   while (data.size() < mRegion.size())
   {
      data.push_back(Aggregator::TextData{glm::vec3(0),glm::vec2(0)});
   }

   mRegion.Set(data.data());
}
   
rhea::linear_expression Text::ConvertTargetToVariable(Engine::UIConstraint::Target target) const
{
   switch(target) {
      case Engine::UIConstraint::ContentWidth:
         return mTextContentWidth;
      case Engine::UIConstraint::ContentHeight:
         return mTextContentHeight;
      default:
         return UIElement::ConvertTargetToVariable(target);
   }
}

void Text::RenderText(const std::string& text)
{
   if (text != mRendered)
   {
      mRendered = text;
      Redraw();
   }
}
   
void Text::RecalculateSize() {
   glm::vec2 size = mFont->GetSizeOfRenderedText(mRendered);

   mpRoot->Suggest(mTextContentWidth, size.x);
   mpRoot->Suggest(mTextContentHeight, size.y);
}

}; // namespace UI

}; // namespace CubeWorld
