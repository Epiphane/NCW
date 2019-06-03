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

   // Here you can pass in an observable that will force the toggle state to whatever you want
   void ProvideToggleSetter(Observables::Observable<bool>& toggler);

private:
   // Internal helper function called whenever mToggled is changed
   void SetToggled(bool newToggle);
   
   // Observable but ONLY for when it is toggled via a click
   DECLARE_OBSERVABLE(bool, mToggled, OnToggled);
   
   // Observable for ALL toggle state changes
   DECLARE_OBSERVABLE(bool, mToggleStateChanged, OnToggleStateSet);

   // Images that appear for their respective toggled states
   Image* mOffImage;
   Image* mOnImage;

   // Internal toggled state
   bool mToggleState = false;
};

}; // namespace Engine

}; // namespace CubeWorld
