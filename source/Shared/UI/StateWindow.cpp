// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Scope.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>

#include "../Aggregator/Image.h"
#include "StateWindow.h"

namespace CubeWorld
{

namespace UI
{

StateWindow::StateWindow(Engine::UIRoot* root, UIElement* parent, std::unique_ptr<Engine::State>&& state)
   : UIElement(root, parent)
   , mState(nullptr)
   , mFramebuffer(Engine::Window::Instance()->GetWidth(), Engine::Window::Instance()->GetHeight())
   , mRegion(root->Reserve<Aggregator::Image>(2))
{
   if (state)
   {
      SetState(std::move(state));
   }

   root->GetAggregator<Aggregator::Image>()->ConnectToTexture(mRegion, mFramebuffer.GetTexture());
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

void StateWindow::Receive(const Engine::UIRebalancedEvent&)
{
   std::vector<Aggregator::ImageData> vertices{
      { mFrame.GetBottomLeft(), glm::vec2(0, 0) },
      { mFrame.GetTopRight(), glm::vec2(1, 1) },
   };

   mRegion.Set(vertices.data());
}

}; // namespace UI

}; // namespace CubeWorld
