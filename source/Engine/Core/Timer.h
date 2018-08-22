// By Thomas Steinke

#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace CubeWorld
{

namespace Engine
{

template<uint32_t N = 1>
class Timer {
public:
   Timer(double gate = 0)
      : mPaused(false)
      , mGate(gate)
      , mCurrentSample(0)
      , mRolling(0)
   {
      mLast = glfwGetTime();
      memset(mSamples, 0, sizeof(mSamples));
   }

   double Elapsed()
   {
      double current = glfwGetTime();
      if (current - mLast < mGate)
      {
         // Nothing to report
         return 0;
      }

      double dt = current - mLast;
      mLast = current;

      mRolling += dt;
      mRolling -= mSamples[mCurrentSample];
      mSamples[mCurrentSample] = dt;
      mCurrentSample = (mCurrentSample + 1) % N;

      return dt;
   }

   double Average()
   {
      return mRolling / N;
   }

   void Reset() { mLast = glfwGetTime(); }

   void Pause() { mPaused = true; }
   void Unpause() { mPaused = false; }
   bool IsPaused() { return mPaused; }

private:
   bool mPaused;

   // Timer gate - don't return time elapsed unless it's greater than this.
   double mGate;
   double mLast;

   size_t mCurrentSample;
   double mRolling;
   double mSamples[N];
};

}; // namespace Engine

}; // namespace CubeWorld
