//
// ButtonVC.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "UIElement.h"

namespace CubeWorld
{

namespace Engine
{

class UITapGestureRecognizer; // Forward declare

class ButtonVC : public UIElement
{
public:
   ButtonVC(UIRoot* root, UIElement* parent, const std::string &name);

   // Sends a message whenever the user completes a tap on this button
   Observables::Observable<UIGestureRecognizer::Message_GestureState>& OnClick();

protected:
   std::shared_ptr<UITapGestureRecognizer> mTapGestureRecognizer;
};

}; // namespace Engine

}; // namespace CubeWorld
