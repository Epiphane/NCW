//
// ButtonVC.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "ButtonVC.h"

#include "UIClickGestureRecognizer.h"

namespace CubeWorld
{

namespace Engine
{

ButtonVC::ButtonVC(UIRootDep* root, UIElementDep* parent, const std::string &name)
   : UIElementDep(root, parent, name)
{
   mTapGestureRecognizer = CreateAndAddGestureRecognizer<UIClickGestureRecognizer>();
}

Observables::Observable<UIGestureRecognizer::Message_GestureState>& ButtonVC::OnClick()
{
   return mTapGestureRecognizer->OnClick();
}

}; // namespace Engine

}; // namespace CubeWorld
