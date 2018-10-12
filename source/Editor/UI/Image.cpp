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

namespace Editor
{

Image::Image(Engine::UIRoot* root, Engine::UIElement* parent, const Options& options)
   : Engine::UIElement(root, parent)
   , mRegion(root->Reserve<Engine::Aggregator::Image>(2))
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
}

void Image::UpdateRegion()
{
   std::vector<Engine::Aggregator::ImageData> vertices{
      {
         glm::vec2(mFrame.left.int_value(), mFrame.bottom.int_value()),
         glm::vec2(mCoords.x, mCoords.y),
      },
      {
         glm::vec2(mFrame.right.int_value(), mFrame.top.int_value()),
         glm::vec2(mCoords.x + mCoords.z, mCoords.y + mCoords.w),
      },
   };

   mRegion.Set(vertices.data());
}

}; // namespace Editor

}; // namespace CubeWorld
