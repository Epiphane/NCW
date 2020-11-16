//
// ButtonVC.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "UIElementDep.h"

namespace CubeWorld
{

namespace Engine
{

class UIClickGestureRecognizer; // Forward declare

class ButtonVC : public UIElementDep
{
public:
   ButtonVC(UIRootDep* root, UIElementDep* parent, const std::string &name);

   // Sends a message whenever the user completes a click on this button
   Observables::Observable<UIGestureRecognizer::Message_GestureState>& OnClick();

protected:
   std::shared_ptr<UIClickGestureRecognizer> mTapGestureRecognizer;
};

}; // namespace Engine

}; // namespace CubeWorld
