// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>

#include <Engine/Core/Config.h>
#include <Engine/Core/Bounded.h>

namespace CubeWorld
{

namespace Engine
{

//
// Allows for making an element bound to a specific value.
//
// For example, TextField inherits from Binding<std::string>, allowing it to
// set the value of a string.
//
template <typename T>
class Binding
{
public:
   Binding() : mBinding(nullptr) {}
   virtual ~Binding() = default;

   //
   // Update the current state of the binding. Returns true if an update was performed.
   //
   bool Update()
   {
      if (!mBinding) { return false; }
      if (*mBinding == mInternalValue)
      {
         return false;
      }
      mInternalValue = *mBinding;
      return true;
   }

   ///
   ///
   ///
   void Bind(T* location)
   {
      mBinding = location;
      if (location != nullptr)
      {
         mInternalValue = *location;
      }
   }

   ///
   ///
   ///
   bool IsBound() { return mBinding != nullptr; }

   ///
   ///
   ///
   T GetValue() { return mInternalValue; }

protected:
   ///
   ///
   ///
   virtual void SetValue(T value)
   {
      mInternalValue = value;
      if (mBinding != nullptr)
      {
         *mBinding = value;
      }
   }

private:
   T* mBinding;
   T mInternalValue;
};

}; // namespace Editor

}; // namespace CubeWorld
