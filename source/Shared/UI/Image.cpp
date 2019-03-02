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
   Engine::UIConstraint::Options opts;
   opts.priority = Engine::UIConstraint::MEDIUM_PRIORITY;
   ConstrainAspectRatio(pixelW / pixelH, opts);

   root->GetAggregator<Aggregator::Image>()->ConnectToTexture(mRegion, mTexture->GetTexture());
}

void Image::SetImage(std::string imageName)
{
   mCoords = mTexture->GetImage(imageName);
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
   
rhea::linear_expression Image::ConvertTargetToVariable(Engine::UIConstraint::Target target) const
{
   switch(target) {
      case Engine::UIConstraint::ContentWidth:
         return mImageContentWidth;
      case Engine::UIConstraint::ContentHeight:
         return mImageContentHeight;
      default:
         return UIElement::ConvertTargetToVariable(target);
   }
}

}; // namespace UI

}; // namespace CubeWorld
