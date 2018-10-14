// By Thomas Steinke

#pragma once

#include <Engine/Event/InputEvent.h>

#include "Image.h"

namespace CubeWorld
{

namespace UI
{

class Button : public Image
{
public:
   struct Options : public Image::Options {
      std::string hoverImage = "";
      std::string pressImage = "";
      std::function<void(void)> onClick = nullptr;
   };

public:
   Button(Engine::UIRoot* root, Engine::UIElement* parent, const Options& options);

   //
   // Render the framebuffer to this subwindow's location.
   //
   void Update(TIMEDELTA dt) override;

   void Receive(const MouseDownEvent& evt);
   void Receive(const MouseUpEvent& evt);

private:
   std::function<void(void)> mCallback;

private:
   enum State { NORMAL, HOVER, PRESS };
   void SetState(State state);

private:
   glm::vec4 mHover;
   glm::vec4 mPress;
   glm::vec4 mNormal;

   State mState;
};

}; // namespace UI

}; // namespace CubeWorld
