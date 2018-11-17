// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>

#include <Engine/Core/Timer.h>
#include <Engine/Graphics/Framebuffer.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/DebugHelper.h>

#include "../Aggregator/Image.h"

namespace CubeWorld
{

namespace UI
{

class SubFrameUIRoot : public Engine::UIRoot
{
public:
   void Receive(const Engine::ElementAddedEvent& evt) override;
   void Receive(const Engine::ElementRemovedEvent& evt) override;

private:
   // Keep track of the dimensions this UIRoot actually occupies
   rhea::variable mLeft, mRight, mTop, mBottom;
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

   //
   // Add a new element of type E to the sub-UI.
   //
   template <typename E, typename ...Args>
   E* Add(Args ...args)
   {
      return mUIRoot.Add<E>(std::forward<Args>(args)...);
   }

public:
   // Functions that get passed through to the UIRoot.

   //
   // Add arbitrary contraints.
   //
   void AddConstraints(const rhea::constraint_list& constraints);

public:
   Engine::UIRoot& GetUI() { return mUIRoot; }

   Engine::UIFrame& GetInnerFrame() { return mUIRoot.GetFrame(); }

private:
   Engine::Timer<100> mUpdateTimer;

   SubFrameUIRoot mUIRoot;

private:
   Engine::Graphics::Framebuffer mFramebuffer;
   glm::tvec2<uint32_t> mScroll;
   Aggregator::Image::Region mRegion;

   std::unique_ptr<DebugHelper::MetricLink> metric;
};

}; // namespace UI

}; // namespace CubeWorld
