// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Scope.h>
#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/Helpers/Asset.h>

#include "Text.h"

namespace CubeWorld
{

namespace Editor
{

Text::Text(Engine::UIRoot* root, UIElement* parent, const Options& options)
   : UIElement(root, parent)
   , mText("")
   , mRendered("")
   , mRegion(root->Reserve<Engine::Aggregator::Text>(2 * options.text.size() + 4))
{
   auto maybeFont = Engine::Graphics::FontManager::Instance()->GetFont(Asset::Font(options.font));
   assert(maybeFont);
   mFont = *maybeFont;

   uint32_t size = options.size;
   if (size == 0)
   {
      size = options.text.size() + 2;
   }

   SetText(options.text);
   root->GetAggregator<Engine::Aggregator::Text>()->ConnectToTexture(mRegion, mFont->GetTexture());
}

void Text::Receive(const Engine::UIRebalancedEvent& evt)
{
   RenderText(mRendered);
}

void Text::RenderText(const std::string& text)
{
   mRendered = text;
   std::vector<Engine::Graphics::Font::CharacterVertexUV> uvs = mFont->Write(mFrame.left.int_value(), mFrame.bottom.int_value(), 1, mRendered);
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

}; // namespace Editor

}; // namespace CubeWorld
