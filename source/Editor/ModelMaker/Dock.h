// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Command.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Receiver.h>
#include <Engine/UI/UIElement.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/Image.h>
#include <Shared/UI/TextButton.h>
#include <Shared/UI/TextField.h>

#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

using UI::Image;
using UI::Text;
using UI::TextField;

class Dock : public Engine::UIElement {
public:
   Dock(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

public:
   // Dock state actions

private:
   // Helper functions

public:
   // Event handlers
   void Receive(const Engine::ComponentAddedEvent<CubeModel>& evt);

private:
   // State

private:
   // General state info

private:
   // Commands

   //
   //
   //
   class DockCommand : public Command {
   public:
      DockCommand(Dock* dock) : dock(dock) {};

   protected:
      Dock* dock;
   };
};

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld
