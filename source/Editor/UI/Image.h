// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Aggregator/Image.h>
#include <Engine/Core/Bounded.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/UI/UIElement.h>

namespace CubeWorld
{

namespace Editor
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

   void UpdateRegion();

   void Receive(const Engine::UIRebalancedEvent&) override
   {
      UpdateRegion();
   }

protected:
   Engine::Graphics::Texture* mTexture;
   glm::vec4 mCoords;

private:
   Engine::Aggregator::Image::Region mRegion;
};

}; // namespace Editor

}; // namespace CubeWorld
