// By Thomas Steinke

#pragma once

#include <cassert>
#include <glm/glm.hpp>

#include <Engine/Core/Input.h>
#include <Engine/Core/Timer.h>
#include <Engine/Event/MouseInputTransformer.h>
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
   SubFrameUIRoot(Engine::Input* input) : UIRoot(input) {}

public:
   void Receive(const Engine::ElementAddedEvent& evt) override;

private:
   // Keep track of the dimensions this UIRoot actually occupies
   rhea::variable mLeft, mRight, mTop, mBottom;
};

//
// SubFrame - an extension of UIRoot, that renders its whole UI to a framebuffer
// before rendering to the screen. The reason for this is so that its elements can
// overflow the screen space smoothly and allow for scrolling.
//
class SubFrame
   : public Engine::UIElement
   , public Engine::Input
   , public MouseInputTransformer
   , public Engine::Transformer<Engine::ElementAddedEvent>
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

public:
   //
   // Overrides from Input base class.
   //
   void Reset() override;
   void Update() override;
   bool IsKeyDown(int key) const override;
   bool IsDragging(int button) const override;
   glm::tvec2<double> GetRawMousePosition() const override;
   glm::tvec2<double> GetMousePosition() const override;
   glm::tvec2<double> GetMouseMovement() const override;
   glm::tvec2<double> GetMouseScroll() const override;
   void SetMouseLock(bool) override { assert(false); }
   bool IsMouseLocked() const override;

private:
   bool mMousePressed[GLFW_MOUSE_BUTTON_LAST];
   bool mMouseDragging[GLFW_MOUSE_BUTTON_LAST];
   glm::tvec2<double> mMousePressOrigin[GLFW_MOUSE_BUTTON_LAST];

public:
   //
   // Transformer overrides.
   //
   const MouseDownEvent TransformEventDown(const MouseDownEvent& evt) const override;
   const MouseUpEvent TransformEventDown(const MouseUpEvent& evt) const override;
   const MouseClickEvent TransformEventDown(const MouseClickEvent& evt) const override;
   bool ShouldPropagateDown(const Engine::ElementAddedEvent&) const override { return false; }
   bool ShouldPropagateUp(const Engine::ElementAddedEvent&) const override { return false; }
};

}; // namespace UI

}; // namespace CubeWorld
