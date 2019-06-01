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
     Observables::OnMessage<UIGestureRecognizer::Message_GestureState>([&](UIGestureRecognizer::Message_GestureState m) {
        mToggleState = !mToggleState;
        mUserToggledObservable.SendMessage(mToggleState);
     }, mBag);
   
   mUserToggledObservable.OnChanged() >>
     Observables::OnMessage<bool>(std::bind(&ToggleButtonVC::Toggled, this, std::placeholders::_1), mBag);
}

void ToggleButtonVC::Toggled(bool newToggle)
{
   mOnImage->SetActive(newToggle);
   mOffImage->SetActive(!newToggle);
}

Observables::Observable<bool>& ToggleButtonVC::OnUserToggled()
{
   return mUserToggledObservable.OnChanged();
}

void ToggleButtonVC::ProvideToggleForcer(Observables::Observable<bool>& toggler)
{
   toggler >>
      Observables::OnMessage<bool>([&](bool toggled) {
         mForcedToggleObservable.SendMessage(toggled);
      }, mBag) >>
      Observables::OnMessage<bool>(std::bind(&ToggleButtonVC::Toggled, this, std::placeholders::_1), mBag);
}

Observables::Observable<bool>& ToggleButtonVC::OnToggleForciblyChanged()
{
   return mForcedToggleObservable.OnChanged();
}

}; // namespace Engine

}; // namespace CubeWorld
