// By Thomas Steinke

#pragma once

#include <memory>

#include <Engine/Event/EventManager.h>
#include <Engine/UI/UIRoot.h>

#include "../Controls.h"
#include "../Imgui/StateWindow.h"
#include "Dock.h"
#include "Sidebar.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

class Editor : public Engine::UIRoot
{
public:
   Editor(Engine::Input* input, const Controls::Options& options);

   //
   // Called every time this editor is reactivated.
   //
   void Start();

   //
   // Called once per frame.
   //
   void Update(TIMEDELTA dt) override;

private:
   Engine::Input& mInput;
   Engine::EventManager mEvents;

   std::unique_ptr<StateWindow> mStateWindow;
};

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld
