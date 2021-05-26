// By Thomas Steinke

#pragma once

#include <memory>

#include <Engine/Event/EventManager.h>
#include <Engine/UI/UIRoot.h>

#include <Shared/Imgui/StateWindow.h>

namespace CubeWorld
{

namespace Editor
{

namespace ShaderLand
{

class Editor : public Engine::UIRoot
{
public:
   Editor(Engine::Input& input);

   //
   // Called every time this editor is reactivated.
   //
   void Start();

   //
   // Called once per frame.
   //
   void Update(TIMEDELTA dt) override;

private:
   Engine::EventManager mEvents;

   std::unique_ptr<StateWindow> mStateWindow;
};

}; // namespace ShaderLand

}; // namespace Editor

}; // namespace CubeWorld
