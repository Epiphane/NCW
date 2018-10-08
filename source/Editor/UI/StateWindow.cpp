// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Scope.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>
#include <Engine/UI/ImageAggregator.h>

#include "StateWindow.h"

namespace CubeWorld
{

namespace Editor
{

StateWindow::StateWindow(Engine::UIRoot* root, UIElement* parent, std::unique_ptr<Engine::State>&& state)
   : UIElement(root, parent)
   , mState(nullptr)
   , mFramebuffer(Engine::Window::Instance()->GetWidth(), Engine::Window::Instance()->GetHeight())
   , mRegion(root->Reserve<Engine::TextAggregator>(2))
{
   if (state)
   {
      SetState(std::move(state));
   }

   root->Subscribe<Engine::UIRebalancedEvent>(*this);
   root->GetAggregator<Engine::ImageAggregator>()->ConnectToTexture(mRegion, mFramebuffer.GetTexture());
}

void StateWindow::SetState(std::unique_ptr<Engine::State>&& state)
{
   mState = std::move(state);
   mState->EnsureLoaded();
}

void StateWindow::Update(TIMEDELTA dt)
{
   // Draw elements
   mFramebuffer.Bind();
   mState->Update(dt);
   mFramebuffer.Unbind();
}

void StateWindow::Receive(const Engine::UIRebalancedEvent& evt)
{
   std::vector<Engine::Graphics::Font::CharacterVertexUV> vertices{
      {
         glm::vec2(mFrame.left.int_value(), mFrame.bottom.int_value()),
         glm::vec2(0, 0),
      },
      {
         glm::vec2(mFrame.right.int_value(), mFrame.top.int_value()),
         glm::vec2(1, 1),
      },
   };

   mRegion.Set(vertices.data());
}

}; // namespace Editor

}; // namespace CubeWorld
