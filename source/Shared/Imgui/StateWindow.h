// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <imgui.h>
#include <Engine/Core/Bounded.h>
#include <Engine/Core/Input.h>
#include <Engine/Core/State.h>
#include <Engine/Event/EventManager.h>
#include <Engine/Event/MouseInputTransformer.h>
#include <Engine/Graphics/Framebuffer.h>

namespace CubeWorld
{

//
// Renders a state to a framebuffer, for translating before rendering to the screen.
//
// Additionally, acts as a "middleman" input manager, for translating global-level input
// (for example, mouse position outside of the state) to state-relative input. This is
// important, because all systems/components/logic in a state should act identically,
// regardless of whether its the core of the window or we've got a StateWindow housing it.
//
// Similar to UI::StateWindow, except playing with Imgui instead of constraints
//
class StateWindow : public Bounds, public Engine::Input, public MouseInputTransformer, public Engine::EventManager
{
public:
   StateWindow(
      Engine::Input& input,
      uint32_t width,
      uint32_t height,
      std::unique_ptr<Engine::State>&& state
   );
   ~StateWindow() {}

   //
   // Add the state to this window. Does not support nice transitions, so don't do any crazy swappin'
   //
   void SetState(std::unique_ptr<Engine::State>&& state);

   //
   // Update the state and render it to the internal framebuffer.
   //
   void Update(TIMEDELTA dt);

   Engine::Graphics::Framebuffer& GetFramebuffer() { return mFramebuffer; }
   void SetPosition(uint32_t x, uint32_t y);
   void SetPosition(ImVec2 position);
   void SetPosition(glm::tvec2<double> position);
   void SetSize(uint32_t w, uint32_t h);
   void SetSize(ImVec2 size);
   void SetSize(glm::tvec2<double> size);

private:
   Engine::Input& mInput;
   std::unique_ptr<Engine::State> mState;
   Engine::Graphics::Framebuffer mFramebuffer;

public:
   //
   // Overrides from Input base class.
   //
   void Reset() override;
   void Update() override;
   bool IsKeyDown(int key) const override;
   bool IsMouseDown(int button) const override;
   bool IsDragging(int button) const override;
   glm::tvec2<double> GetRawMousePosition() const override;
   glm::tvec2<double> GetMousePosition() const override;
   glm::tvec2<double> CorrectYCoordinate(glm::tvec2<double> point) const override;
   glm::tvec2<double> GetMouseMovement() const override;
   glm::tvec2<double> GetMouseScroll() const override;
   void SetMouseLock(bool locked) override;
   bool IsMouseLocked() const override;

private:
    bool mMousePressed[GLFW_MOUSE_BUTTON_LAST] = {false};
    bool mMouseDragging[GLFW_MOUSE_BUTTON_LAST] = {false};
    glm::tvec2<double> mMousePressOrigin[GLFW_MOUSE_BUTTON_LAST] = {{0,0}};

public:
   //
   // Transformer overrides.
   //
   const MouseDownEvent TransformEventDown(const MouseDownEvent& evt) const override;
   const MouseUpEvent TransformEventDown(const MouseUpEvent& evt) const override;
   const MouseClickEvent TransformEventDown(const MouseClickEvent& evt) const override;
};

}; // namespace CubeWorld
