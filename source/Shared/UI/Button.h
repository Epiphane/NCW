// By Thomas Steinke

#pragma once

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

   Action MouseDown(const MouseDownEvent& evt) override;
   Action MouseMove(const MouseMoveEvent& evt) override;
   Action MouseUp(const MouseUpEvent& evt) override;

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
