// By Thomas Steinke

#include <algorithm>
#include <imgui.h>

#include <RGBDesignPatterns/CommandStack.h>
#include <RGBText/StringHelper.h>
#include <Engine/UI/UIRoot.h>

#include <Shared/Imgui/Extensions.h>
#include <Shared/Imgui/Meta.h>

#include "Dock.h"

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
{

Dock::Dock(Engine::UIRoot* root, Engine::UIElement* parent)
   : Engine::UIElement(root, parent)
{
   root->Subscribe<ParticleEmitterLoadedEvent>(*this);
}

///
///
///
void Dock::Update(TIMEDELTA)
{
   if (!mParticleSystem)
   {
      return;
   }

   ImGui::SetNextWindowPos(ImVec2(1000, 20), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowSize(ImVec2(250, 700), ImGuiCond_FirstUseEver);
   ImGui::Begin("Particle");

   ImGui::Text("%s", mParticleSystem->name.c_str());

   Engine::ParticleSystem::Shape shape = mParticleSystem->shape;

   if (Imgui::Draw("", static_cast<Engine::ParticleSystem&>(*mParticleSystem)))
   {
      if (shape != mParticleSystem->shape)
      {
         mParticleSystem->Reset();
      }
   }

   ImGui::End();
}

///
///
///
void Dock::Receive(const ParticleEmitterLoadedEvent& evt)
{
   mParticleSystem = evt.component;
}

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld
