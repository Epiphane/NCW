// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <RGBDesignPatterns/Command.h>
#include <Engine/UI/UIElement.h>

#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
{

class Dock : public Engine::UIElement {
public:
   Dock(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

public:
   // Dock state actions

public:
   // Event handlers
   void Receive(const ParticleEmitterLoadedEvent& evt);
   void Receive(const Engine::ComponentAddedEvent<ParticleEmitter>& evt);

private:
   // State
   Engine::ComponentHandle<ParticleEmitter> mParticleSystem;

private:
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

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld
