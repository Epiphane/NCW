//
// Observable.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

namespace CubeWorld
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
   typedef std::function<void(T)> ValueChangedCallback;

   T GetData() const { return internalData; };
   void SetData(T t) {
      internalData = t;

      for (auto callback : mCallbacks) {
         callback(t);
      }
   };

   void AddCallback(ValueChangedCallback newCallback)
   {
      mCallbacks.push_back(newCallback);
   };

private:
   T internalData;
   std::vector<ValueChangedCallback> mCallbacks;
};

} // namespace CubeWorld
