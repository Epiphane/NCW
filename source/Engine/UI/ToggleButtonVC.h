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
   // Constructor with no images. Lets you handle your own toggle state changes.
   ToggleButtonVC(UIRoot* root, UIElement* parent, const std::string& name = "");
   ToggleButtonVC(UIRoot* root, UIElement* parent, Image::Options offImage, Image::Options onImage, const std::string& name = "");

   Observables::Observable<bool>& GetToggleObservable() { return mToggleObservable; }

private:
   // Observable the 'toggled' state of this button
   Observables::Observable<bool> mToggleObservable;

   // Images that appear for their respective toggled states
   Image* mOffImage;
   Image* mOnImage;

   // Internal toggled state
   bool mToggleState;
};

}; // namespace Engine

}; // namespace CubeWorld
