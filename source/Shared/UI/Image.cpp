// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Paths.h>
#include <Engine/Core/Scope.h>
#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>

#include "Image.h"

namespace CubeWorld
{

namespace UI
{

Image::Image(Engine::UIRoot* root, Engine::UIElement* parent, const Options& options)
   : Engine::UIElement(root, parent)
   , mRegion(root->Reserve<Aggregator::Image>(2))
{   
   LOG_DEBUG("Loading %1", Paths::Canonicalize(options.filename));
   Maybe<Engine::Graphics::Texture*> maybeTexture = Engine::Graphics::TextureManager::Instance()->GetTexture(options.filename);
   if (!maybeTexture)
   {
      LOG_ERROR(maybeTexture.Failure().WithContext("Failed loading %1", options.filename).GetMessage());
      return;
   }
   mTexture = *maybeTexture;

   mCoords = glm::vec4(0, 0, 1, 1);
   if (!options.image.empty())
   {
      mCoords = mTexture->GetImage(options.image);
   }

   root->AddConstraints({
      rhea::constraint(mFrame.width * double(mTexture->GetHeight()) == mFrame.height * double(mTexture->GetWidth()), rhea::strength::medium())
   });

   root->GetAggregator<Aggregator::Image>()->ConnectToTexture(mRegion, mTexture->GetTexture());
}

void Image::UpdateRegion()
{
   std::vector<Aggregator::ImageData> vertices{
      { mFrame.GetBottomLeft(), glm::vec2(mCoords.x, mCoords.y) },
      { mFrame.GetTopRight(), glm::vec2(mCoords.x + mCoords.z, mCoords.y + mCoords.w) },
   };

   mRegion.Set(vertices.data());
}

void Image::Receive(const Engine::UIRebalancedEvent& evt)
{
   UIElement::Receive(evt);
   UpdateRegion();
}

}; // namespace UI

}; // namespace CubeWorld
