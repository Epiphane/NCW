// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/UI/UIElement.h>
#include "../Aggregator/Image.h"

namespace CubeWorld
{

namespace UI
{

class Image : public Engine::UIElement
{
public:
   struct Options {
      std::string filename;
      std::string image = "";
   };

public:
   Image(Engine::UIRoot* root, Engine::UIElement* parent, const Options& options);

   void Redraw() override;

protected:
   Engine::Graphics::Texture* mTexture;
   glm::vec4 mCoords;

private:
   Aggregator::Image::Region mRegion;
};

}; // namespace UI

}; // namespace CubeWorld
