// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Command.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Receiver.h>

#include "../UI/SubWindow.h"
#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

class Dock : public SubWindow, public Engine::EventManager, public Engine::Receiver<Dock> {
public:
   Dock(
      Bounded& parent,
      const Options& options
   );

   void Update(TIMEDELTA dt) override;

public:
   // Dock state actions

private:
   // Helper functions

public:
   // Event handlers
   void Receive(const Engine::ComponentAddedEvent<Game::CubeModel>& evt);

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
