// By Thomas Steinke

#pragma once

#include <string>

#include <Engine/UI/UIRoot.h>

#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ShaderLand
{

class Tools : public Engine::UIElement {
public:
    Tools(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

private:
   // Actions
   void LoadNewFile();
   void SaveNewFile();

   void SaveFile();
   void LoadFile(const std::string& filename);

public:
   // Event handlers

private:
   // State
   std::string mFilename;
};

}; // namespace ShaderLand

}; // namespace Editor

}; // namespace CubeWorld
