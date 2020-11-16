// By Thomas Steinke

#pragma once

#include <string>

#include <Engine/UI/UIRoot.h>

#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
{

class Sidebar : public Engine::UIElement {
public:
   Sidebar(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

private:
   // Actions
   void SetModified(bool modified);

   void LoadNewFile();
   void SaveNewFile();

   void SaveFile();
   void LoadFile(const std::string& filename);

public:
   // Event handlers
   void Receive(const Engine::ComponentAddedEvent<ParticleEmitter>& evt);
   void Receive(const ParticleEmitterReadyEvent& evt);
   void Receive(const ParticleEmitterModifiedEvent& evt);

private:
   // State
   std::string mFilename;
   Engine::ComponentHandle<ParticleEmitter> mParticleSystem;
   bool mModified;
};

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld
