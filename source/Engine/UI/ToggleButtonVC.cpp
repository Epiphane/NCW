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
   // TODO-EF: The ContentSize of a ToggleButton (and a Button in general) should be
   //             equal to the size of its background image.
   //
   //          And/or if there is a text label on the button, that should also inform
   //             the content size somehow. I dunno I'll cross that bridge when I get to it.
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
