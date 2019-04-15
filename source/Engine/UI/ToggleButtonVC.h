//
// ToggleButtonVC.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "ButtonVC.h"

#include <Shared/UI/Image.h>

namespace CubeWorld
{

namespace Engine
{
   
using UI::Image;

// My idea:
//  These "Observable" values live in the model or the VC.
//  The view can have a ref to them.
//  When one side changes the Observable value, the other
//    side gets a callback that the value has been updated.
//  (also you can have more than 2 parties involved in this
//    Observable)
//  Could these be something complicated like a UIConstraint??
//    I'm not sure! Let's find out!
template<typename T>
class Observable
{
public:
   typedef std::function<void(T)> ValueChangedCallback;
   
   void SetData(T t) const;
   
   void AddCallback(ValueChangedCallback newCallback) const;
   
private:
   std::vector<ValueChangedCallback> mCallbacks;
};

class ToggleButtonVC : public ButtonVC
{
public:
   ToggleButtonVC(UIRoot* root, UIElement* parent, const Observable<bool>& isOn, Image::Options offImage, Image::Options onImage, const std::string& name = "");

private:
   void Toggled(bool isOn);
   
   Image* mOffImage;
   Image* mOnImage;
   
   // If true, this toggle button is ENABLED
   const Observable<bool>& mIsOn;
};

}; // namespace Engine

}; // namespace CubeWorld
