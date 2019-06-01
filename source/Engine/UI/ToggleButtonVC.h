//
// ToggleButtonVC.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "ButtonVC.h"

#include <Shared/UI/Image.h>
#include <RGBBinding/Observable.h>

namespace CubeWorld
{

namespace Engine
{
   
using UI::Image;

class ToggleButtonVC : public ButtonVC
{
public:
   ToggleButtonVC(UIRoot* root, UIElement* parent, Image::Options offImage, Image::Options onImage, const std::string& name = "");

   // Returns an Observable that sends a message whenever the user clicks this button
   Observables::Observable<bool>& OnUserToggled();

   // Here you can pass in an observable that will force the toggle state to whatever you want
   void ProvideToggler(Observables::Observable<bool>& toggler);

   // Observable for when the toggle is forced to another value by the toggler above
   Observables::Observable<bool>& OnToggleValueChanged();

private:
   // Internal helper function called whenever mToggled is changed
   void Toggled(bool newToggle);

   Observables::ObservableInternal<bool> mUserToggledObservable;
   Observables::ObservableInternal<bool> mProgrammaticToggleObservable;

   // Images that appear for their respective toggled states
   Image* mOffImage;
   Image* mOnImage;

   // Internal toggled state
   bool mToggleState = false;
};

}; // namespace Engine

}; // namespace CubeWorld
