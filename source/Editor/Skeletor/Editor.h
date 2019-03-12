// By Thomas Steinke

#pragma once

#include <memory>

#include <Engine/Event/EventManager.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/StateWindow.h>

#include "../Controls.h"
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

private:
   Engine::EventManager mEvents;

   UI::StateWindow* mStateWindow;
};

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld