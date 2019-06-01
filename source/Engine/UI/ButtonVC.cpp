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
   mTapGestureRecognizer = CreateAndAddGestureRecognizer<UITapGestureRecognizer>();
}

Observables::Observable<UIGestureRecognizer::Message_GestureState>& ButtonVC::OnClick()
{
   return mTapGestureRecognizer->OnTap();
}

}; // namespace Engine

}; // namespace CubeWorld
