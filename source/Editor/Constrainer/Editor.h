// By Thomas Steinke

#pragma once

#include <memory>

#include <Engine/Event/EventManager.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/StateWindow.h>
#include <Shared/Helpers/JsonFileSync.h>

#include "../Controls.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

class Editor : public Engine::UIRoot
{
public:
   Editor(Engine::Input* input, const Controls::Options& options);

   //
   // Called every time this editor is reactivated.
   //
   void Start();

   void TestButton();

   virtual void UpdateRoot() override;

private:
   void BigDumbTest();

   Engine::EventManager mEvents;

   Shared::JsonFileSync mFileSyncer;

//   TextButton* mTestContextMenuButton;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld
