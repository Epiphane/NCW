// By Thomas Steinke

#pragma once

#include <memory>

#include <Engine/Event/EventManager.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/StateWindow.h>

#include "../Controls.h"
//#include "Dock.h"
//#include "Sidebar.h"
//#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

class TextButton;

class Editor : public Engine::UIRoot
{
public:
   Editor(Engine::Input* input, const Controls::Options& options);

   //
   // Called every time this editor is reactivated.
   //
   void Start();

   void TestButton();

private:
   void BigDumbTest();

   Engine::EventManager mEvents;

   TextButton* mTestContextMenuButton;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld
