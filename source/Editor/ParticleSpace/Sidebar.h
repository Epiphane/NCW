// By Thomas Steinke

#pragma once

#include <string>
#include <unordered_map>

#include <Engine/UI/UIElement.h>
#include <Shared/UI/TextButton.h>
#include <Shared/UI/RectFilled.h>

#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
{

class Sidebar : public UI::RectFilled {
public:
   Sidebar(Engine::UIRoot* root, Engine::UIElement* parent);

private:
   // Actions
   void SetModified(bool modified);

   void LoadNewFile();
   void SaveNewFile();

   void SaveFile();
   void LoadFile(const std::string& filename);

   void DiscardChanges();
   void Quit();

private:
   // Elements
   UI::TextButton* mSave;
   UI::TextButton* mQuit;

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
