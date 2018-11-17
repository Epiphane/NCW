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

Image::Image(Engine::UIRoot* root, Engine::UIElement* parent, const Options& options, const std::string& name)
   : Engine::UIElement(root, parent, name)
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

   double pixelW = mCoords.z * mTexture->GetWidth();
   double pixelH = mCoords.w * mTexture->GetHeight();
   root->AddConstraints({
      rhea::constraint(mFrame.width * pixelH == mFrame.height * pixelW, rhea::strength::medium())
   });

   root->GetAggregator<Aggregator::Image>()->ConnectToTexture(mRegion, mTexture->GetTexture());
}

void Image::Redraw()
{
   std::vector<Aggregator::ImageData> vertices;
   if (mActive)
   {
      vertices.push_back({mFrame.GetBottomLeft(), glm::vec2(mCoords.x, mCoords.y + mCoords.w)});
      vertices.push_back({mFrame.GetTopRight(), glm::vec2(mCoords.x + mCoords.z, mCoords.y)});
   }
   else
   {
      vertices.push_back({glm::vec3(0),glm::vec2(0)});
      vertices.push_back({glm::vec3(0),glm::vec2(0)});
   }

   mRegion.Set(vertices.data());
}

}; // namespace UI

}; // namespace CubeWorld
