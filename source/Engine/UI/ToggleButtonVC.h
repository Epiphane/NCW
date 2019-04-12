//
// ToggleButtonVC.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "ButtonVC.h"

namespace CubeWorld
{

namespace Engine
{

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
   void SetData(T t);
   
   void AddCallback(std::function<void(T)> newCallback);
   
private:
   T internalData;
};

typedef Observable<bool> Observabool;  // Dumb joke, ignore

class ToggleButtonVC : public ButtonVC
{
public:
   ToggleButtonVC(UIRoot* root, UIElement* parent, const std::string &name);

   Observabool mIsOn;   // oh god don't actually use that typedef
};

}; // namespace Engine

}; // namespace CubeWorld
