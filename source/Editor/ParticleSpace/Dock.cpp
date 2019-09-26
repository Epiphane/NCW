// By Thomas Steinke

#include <algorithm>
#include <imgui.h>

#include <RGBDesignPatterns/CommandStack.h>
#include <RGBText/StringHelper.h>
#include <Engine/UI/UIStackView.h>
#include <Shared/Helpers/Asset.h>
#include <Shared/UI/Button.h>
#include <Shared/UI/RectFilled.h>

#include "../Imgui/Extensions.h"

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
