//
// ToggleButtonVC.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "ToggleButtonVC.h"

#include <RGBBinding/ObservableBasicOperations.h>

namespace CubeWorld
{

namespace Engine
{

ToggleButtonVC::ToggleButtonVC(UIRoot* root, UIElement* parent, Image::Options offImage, Image::Options onImage, const std::string& name)
   : ButtonVC(root, parent, name)
{
   mOffImage = Add<Image>(offImage);
   mOnImage = Add<Image>(onImage);

   OnClick() >>
     Observables::OnMessage<UIGestureRecognizer::Message_GestureState>([&](auto m) {
        SetToggled(!mToggleState);
        mToggled.SendMessage(mToggleState);
     }, mBag);
}

void ToggleButtonVC::SetToggled(bool newToggle)
{
   mToggleState = newToggle;
   mOnImage->SetActive(newToggle);
   mOffImage->SetActive(!newToggle);
   mToggleStateChanged.SendMessage(newToggle);
}

void ToggleButtonVC::ProvideToggleSetter(Observables::Observable<bool>& toggler)
{
   toggler >>
      Observables::OnMessage<bool>([&](bool toggled) {
         SetToggled(toggled);
      }, mBag);
}

}; // namespace Engine

}; // namespace CubeWorld
