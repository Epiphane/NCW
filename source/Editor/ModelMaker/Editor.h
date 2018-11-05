// By Thomas Steinke

#pragma once

#include <memory>

#include <Engine/Event/EventManager.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/StateWindow.h>

#include "Dock.h"
#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

class Editor : public Engine::UIRoot
{
public:
   Editor(Bounded& parent);

   //
   // Called every time this editor is reactivated.
   //
   void Start();

private:
   Engine::EventManager mEvents;

   UI::StateWindow* mStateWindow;
};

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld
