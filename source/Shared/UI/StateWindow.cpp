// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Scope.h>
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
   , mFramebuffer(root->GetWidth(), root->GetHeight())
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

void StateWindow::Redraw()
{
   mFramebuffer.Resize(GetWidth(), GetHeight());
   std::vector<Aggregator::ImageData> vertices{
      { mFrame.GetBottomLeft(), glm::vec2(0, 0) },
      { mFrame.GetTopRight(), glm::vec2(1, 1) },
   };

   mRegion.Set(vertices.data());
}

//
// Input functions
//
void StateWindow::Reset()
{
   mpRoot->GetInput()->Reset();
}

void StateWindow::Update()
{
   // State is updated in StateWindow::Update(dt) anyway
   assert(false && "My root's input should be updating, not mine!");
}

bool StateWindow::IsKeyDown(int key) const
{
   return mpRoot->GetInput()->IsKeyDown(key);
}

bool StateWindow::IsDragging(int button) const
{
   return mMouseDragging[button];
}

glm::tvec2<double> StateWindow::GetRawMousePosition() const
{
   return mpRoot->GetInput()->GetRawMousePosition() - glm::tvec2<double>{GetX(), GetY()};
}

glm::tvec2<double> StateWindow::GetMousePosition() const
{
   return GetRawMousePosition() / glm::tvec2<double>{GetWidth(), GetHeight()};
}

glm::tvec2<double> StateWindow::GetMouseMovement() const
{
   return mpRoot->GetInput()->GetMouseMovement();
}

glm::tvec2<double> StateWindow::GetMouseScroll() const
{
   return mpRoot->GetInput()->GetMouseScroll();
}

void StateWindow::SetMouseLock(bool locked)
{
   mpRoot->GetInput()->SetMouseLock(locked);
}

bool StateWindow::IsMouseLocked() const
{
   return mpRoot->GetInput()->IsMouseLocked();
}

}; // namespace UI

}; // namespace CubeWorld
