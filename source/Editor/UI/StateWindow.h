// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/State.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/Framebuffer.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/UI/UIElement.h>

namespace CubeWorld
{

namespace Editor
{

//
// Renders a state to a framebuffer, for translating before rendering to the screen.
//
class StateWindow : public Engine::UIElement
{
public:
   StateWindow(Engine::UIRoot* root, UIElement* parent, std::unique_ptr<Engine::State>&& state);
   ~StateWindow() {}

   //
   // Add the state to this window. Does not support nice transitions, so don't do any crazy swappin'
   //
   void SetState(std::unique_ptr<Engine::State>&& state);

   //
   // Add vertices for this window's framebuffer to outVertices.
   //
   virtual void AddVertices(std::vector<Engine::Graphics::Font::CharacterVertexUV>& outVertices) override;

   //
   // Update the state and render it to the internal framebuffer.
   //
   void Update(TIMEDELTA dt) override;

   //
   // Render the state to the screen
   //
   virtual size_t Render(Engine::Graphics::VBO& vbo, size_t offset) override;

private:
   std::unique_ptr<Engine::State> mState;
   Engine::Graphics::Framebuffer mFramebuffer;

private:
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Editor

}; // namespace CubeWorld
