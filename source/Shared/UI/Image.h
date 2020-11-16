// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/UI/UIElementDep.h>
#include "../Aggregator/Image.h"

namespace CubeWorld
{

namespace UI
{

class Image : public Engine::UIElementDep
{
public:
   struct Options {
      std::string filename;
      std::string image = "";
   };

public:
   Image(Engine::UIRootDep* root, Engine::UIElementDep* parent, const Options& options, const std::string& name = "");

   // Set new image UVs on the same texture
   void SetImage(std::string imageName);
   void Redraw() override;

   rhea::linear_expression ConvertTargetToVariable(Engine::UIConstraint::Target target) const override;

protected:
   Engine::Graphics::Texture* mTexture;
   glm::vec4 mCoords;

   Aggregator::Image::Region mRegion;

   // Edit variables that let you make constraints to the texture size.
   //    Will change their values whenever a new image is specified.
   rhea::variable mImageContentWidth;
   rhea::variable mImageContentHeight;

   // Set mImageContentWidth and mImageContentHeight to the size of the rendered texture.
   void UpdateContentSize();

};

}; // namespace UI

}; // namespace CubeWorld
