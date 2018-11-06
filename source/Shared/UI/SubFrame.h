// By Thomas Steinke

#pragma once

#include <Engine/Graphics/Framebuffer.h>
#include <Engine/UI/UIRoot.h>

#include "../Aggregator/Image.h"

namespace CubeWorld
{

namespace UI
{

class SubFrameUI : public Engine::UIRoot
{
   void Receive(const Engine::ElementAddedEvent& evt) override;
   void Receive(const Engine::ElementRemovedEvent& evt) override;
};

//
// SubFrame - an extension of UIRoot, that renders its whole UI to a framebuffer
// before rendering to the screen. The reason for this is so that its elements can
// overflow the screen space smoothly and allow for scrolling.
//
class SubFrame : public Engine::UIElement
{
public:
   SubFrame(Engine::UIRoot* root, UIElement* parent);
   ~SubFrame() {}

   //
   // Update the state and render it to the internal framebuffer.
   //
   void Update(TIMEDELTA dt) override;

   //
   // Rebuffer the region data, for example when the UI is rebalanced.
   //
   void Redraw() override;

public:
   Engine::UIRoot& GetUI() { return mUIRoot; }

private:
   Engine::UIRoot mUIRoot;

private:
   Engine::Graphics::Framebuffer mFramebuffer;
   Aggregator::Image::Region mRegion;
};

}; // namespace UI

}; // namespace CubeWorld
