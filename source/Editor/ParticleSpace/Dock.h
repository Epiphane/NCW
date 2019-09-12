// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <RGBDesignPatterns/Command.h>
#include <Engine/UI/UIElement.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/Image.h>
#include <Shared/UI/NumDisplay.h>
#include <Shared/UI/RectFilled.h>
#include <Shared/UI/ScrollBar.h>
#include <Shared/UI/TextButton.h>
#include <Shared/UI/TextField.h>

#include "../UI/Scrubber.h"
#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
{

using UI::Image;
using UI::NumDisplay;
using UI::RectFilled;
using UI::ScrollBar;
using UI::Text;
using UI::TextField;

class Dock : public RectFilled {
public:
   Dock(Engine::UIRoot* root, Engine::UIElement* parent);

public:
   // Dock state actions

public:
   // Event handlers
   void Receive(const ClearParticleEmitterEvent& evt);
   void Receive(const ParticleEmitterLoadedEvent& evt);
   void Receive(const Engine::ComponentAddedEvent<ParticleEmitter>& evt);

private:
   // State
   std::unique_ptr<Command> mScrubbing;
   Engine::ComponentHandle<ParticleEmitter> mParticleSystem;

private:
   template <typename N>
   struct LabelAndScrubber {
      NumDisplay<N>* text;
      Dep::Scrubber<N>* scrubber;
   };

   // Stance inspector
   Text* mParticleName;

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

   //
   //
   //
   class SetStanceNameCommand : public DockCommand
   {
   public:
      SetStanceNameCommand(Dock* dock, std::string name) : DockCommand(dock), name(name) {};
      void Do() override;
      void Undo() override { Do(); }

   private:
      std::string name;
   };
};

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld
