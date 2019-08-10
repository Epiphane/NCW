// By Thomas Steinke

#include <algorithm>

#include <RGBText/StringHelper.h>
#include <Engine/UI/UIStackView.h>
#include <Shared/Helpers/Asset.h>
#include <Shared/UI/Button.h>
#include <Shared/UI/RectFilled.h>

#include "../Command/CommandStack.h"

#include "Dock.h"

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
{

using Engine::UIElement;
using Engine::UIFrame;
using Engine::UIStackView;
using UI::Button;
using UI::RectFilled;

Dock::Dock(Engine::UIRoot* root, UIElement* parent)
   : RectFilled(root, parent, "ParticleSpaceDock", glm::vec4(0.2, 0.2, 0.2, 1))
   , mParticleName(nullptr)
{
   RectFilled* foreground = Add<RectFilled>("ParticleSpaceDockFG", glm::vec4(0, 0, 0, 1));

   foreground->ConstrainCenterTo(this);
   foreground->ConstrainDimensionsTo(this, -4);

   // ParticleEmitter information
   UIElement* header = Add<UIStackView>();
   header->ConstrainLeftAlignedTo(foreground, 32);
   header->ConstrainTopAlignedTo(foreground, 32);
   {
      UIStackView* row1 = header->Add<UIStackView>();
      row1->SetVertical(false);
      row1->SetOffset(8.0);
      row1->ConstrainHeight(19);
      row1->ConstrainLeftAlignedTo(header);

      mParticleName = row1->Add<Text>(Text::Options{"Name"});
      mParticleName->ConstrainTopAlignedTo(row1);
      mParticleName->ConstrainHeightTo(row1);
   }

   root->Subscribe<ClearParticleEmitterEvent>(*this);
   root->Subscribe<ParticleEmitterLoadedEvent>(*this);
}

///
///
///
void Dock::Receive(const ClearParticleEmitterEvent&)
{
   if (mParticleName != nullptr)
   {
      mParticleName->SetText("");
   }
}

///
///
///
void Dock::Receive(const ParticleEmitterLoadedEvent& evt)
{
   mParticleSystem = evt.component;

   if (mParticleName != nullptr)
   {
      // mParticleName->SetText(mParticleSystem->GetName());
   }

   // Clear all bindings
   Receive(ClearParticleEmitterEvent{});
}

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld
