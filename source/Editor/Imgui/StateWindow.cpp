// By Thomas Steinke

#include <cassert>

#include <RGBDesignPatterns/Scope.h>
#include <RGBLogger/Logger.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/Aggregator/Image.h>

#include "StateWindow.h"

namespace CubeWorld
{

namespace Editor
{

StateWindow::StateWindow(
   Engine::Input& input,
   uint32_t width,
   uint32_t height,
   Aggregator::Image& aggregator,
   std::unique_ptr<Engine::State>&& state
)  : Bounds(0, 0, width, height)
   , mInput(input)
   , mState(nullptr)
   , mFramebuffer((GLsizei)GetWidth(), (GLsizei)GetHeight())
   , mRegion(aggregator.Reserve(2))
{
   if (state)
   {
      SetState(std::move(state));
   }

   aggregator.ConnectToTexture(mRegion, mFramebuffer.GetTexture());
}

void StateWindow::SetState(std::unique_ptr<Engine::State>&& state)
{
   mState = std::move(state);
   mState->EnsureLoaded();
}

void StateWindow::Update(TIMEDELTA dt)
{
   // Draw elements
   mFramebuffer.Resize((GLsizei)GetWidth(), (GLsizei)GetHeight());
   mFramebuffer.Bind();
   mState->Update(dt);
   mFramebuffer.Unbind();
}

//
// Input functions
//
void StateWindow::Reset()
{
   mInput.Reset();
}

void StateWindow::Update()
{
   // State is updated in StateWindow::Update(dt) anyway
   assert(false && "My root's input should be updating, not mine!");
}

void StateWindow::SetPosition(uint32_t x, uint32_t y)
{
   this->x = x;
   this->y = y;
}

void StateWindow::SetPosition(ImVec2 position)
{
   SetPosition((uint32_t)position.x, (uint32_t)position.y);
}

void StateWindow::SetPosition(glm::tvec2<double> position)
{
   SetPosition((uint32_t)position.x, (uint32_t)position.y);
}

void StateWindow::SetSize(uint32_t w, uint32_t h)
{
   this->w = w;
   this->h = h;
}

void StateWindow::SetSize(ImVec2 size)
{
   SetSize((uint32_t)size.x, (uint32_t)size.y);
}

void StateWindow::SetSize(glm::tvec2<double> size)
{
   SetSize((uint32_t)size.x, (uint32_t)size.y);
}

bool StateWindow::IsKeyDown(int key) const
{
   return mInput.IsKeyDown(key);
}

bool StateWindow::IsMouseDown(int button) const
{
   return mMousePressed[button];
}

bool StateWindow::IsDragging(int button) const
{
   return mMouseDragging[button];
}

glm::tvec2<double> StateWindow::GetRawMousePosition() const
{
   return mInput.GetRawMousePosition() - glm::tvec2<double>{GetX(), GetY()};
}

glm::tvec2<double> StateWindow::GetMousePosition() const
{
   return GetRawMousePosition() / glm::tvec2<double>{GetWidth(), GetHeight()};
}

glm::tvec2<double> StateWindow::CorrectYCoordinate(glm::tvec2<double> point) const
{
   return mInput.CorrectYCoordinate(point);
}

glm::tvec2<double> StateWindow::GetMouseMovement() const
{
   return mInput.GetMouseMovement();
}

glm::tvec2<double> StateWindow::GetMouseScroll() const
{
   return mInput.GetMouseScroll();
}

void StateWindow::SetMouseLock(bool locked)
{
   mInput.SetMouseLock(locked);
}

bool StateWindow::IsMouseLocked() const
{
   return mInput.IsMouseLocked();
}

//
// Transformer overrides.
//
const MouseDownEvent StateWindow::TransformEventDown(const MouseDownEvent& evt) const
{
   return MouseDownEvent(evt.button, (evt.x - GetX()) / GetWidth(), (evt.y - GetY()) / GetHeight());
}

const MouseUpEvent StateWindow::TransformEventDown(const MouseUpEvent& evt) const
{
   return MouseUpEvent(evt.button, (evt.x - GetX()) / GetWidth(), (evt.y - GetY()) / GetHeight());
}

const MouseClickEvent StateWindow::TransformEventDown(const MouseClickEvent& evt) const
{
   return MouseClickEvent(evt.button, (evt.x - GetX()) / GetWidth(), (evt.y - GetY()) / GetHeight());
}

}; // namespace Editor

}; // namespace CubeWorld
