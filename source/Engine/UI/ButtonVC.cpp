//
// ButtonVC.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "ButtonVC.h"

#include "UITapGestureRecognizer.h"

namespace CubeWorld
{

namespace Engine
{

ButtonVC::ButtonVC(UIRoot* root, UIElement* parent, const std::string &name)
   : UIElement(root, parent, name)
{
   Engine::GestureCallback tapCallback = std::bind(&ButtonVC::TapHandler, this, std::placeholders::_1);
   CreateAndAddGestureRecognizer<UITapGestureRecognizer>(tapCallback);
}
   
void ButtonVC::TapHandler(const Engine::UIGestureRecognizer& rec)
{
   switch (rec.GetState()) {
      case Engine::UIGestureRecognizer::Starting:
         break;
      case Engine::UIGestureRecognizer::Possible:
      case Engine::UIGestureRecognizer::Cancelled:
         break;
      case Engine::UIGestureRecognizer::Ending:
         OnClick();
         break;
      case Engine::UIGestureRecognizer::Happening:
         // Do nothing, we're already depressed
         break;
   }
}
   
void ButtonVC::SetCallback(std::function<void(void)> callback) {
   mClickCallback = callback;
}
   
void ButtonVC::OnClick()
{
   mClickCallback();
}

}; // namespace Engine

}; // namespace CubeWorld
