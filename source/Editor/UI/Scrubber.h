// By Thomas Steinke

#pragma once

#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/Command.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/Binding.h>
#include <Engine/UI/UIElement.h>

#include "Image.h"

namespace CubeWorld
{

namespace Editor
{

//
// This is just convenience, so I can hide some implementation in Scrubber.cpp. Always use the derived Scrubber<N> class
//
class BaseScrubber : public Image
{
public:
   BaseScrubber(Engine::UIRoot* root, Engine::UIElement* parent, const Options& options);

   //
   // Render the framebuffer to this subwindow's location.
   //
   void Update(TIMEDELTA dt) override;

   void Receive(const MouseDownEvent& evt);
   void Receive(const MouseUpEvent& evt);

   bool IsScrubbing() { return mScrubbing != nullptr; }

private:
   virtual void StartScrubbing() = 0;
   virtual void Scrub(double amount) = 0;

protected:
   std::unique_ptr<Command> mScrubbing;
   glm::tvec2<double> mLastPosition;
};

template <typename N>
class Scrubber : public BaseScrubber, public Engine::Binding<N>
{
public:
   ///
   ///
   ///
   struct Options : public BaseScrubber::Options {
      N min = std::numeric_limits<N>::min();
      N max = std::numeric_limits<N>::max();
      std::function<void(N, N)> onChange = nullptr;
   };

   Scrubber(Engine::UIRoot* root, Engine::UIElement* parent, const Options& options)
      : BaseScrubber(root, parent, options)
      , mMin(options.min)
      , mMax(options.max)
      , mCallback(options.onChange)
   {};

   void Update(TIMEDELTA dt) override
   {
      Engine::Binding<N>::Update();
      BaseScrubber::Update(dt);
   }

private:
   // Allows for undoing.
   class ScrubCommand : public Command {
   public:
      ScrubCommand(Scrubber* scrubber, std::function<void(N, N)> callback)
         : scrubber(scrubber)
         , value(scrubber->GetValue())
         , callback(callback)
      {};
      void Do() override
      {
         N newValue = value;
         value = scrubber->GetValue();
         scrubber->SetValue(newValue);
         if (callback)
         {
            callback(newValue, value);
         }
      }
      void Undo() override { Do(); }

   protected:
      Scrubber* scrubber;
      N value;
      std::function<void(N, N)> callback;
   };

   void StartScrubbing() override
   {
      mScrubbing = std::make_unique<ScrubCommand>(this, mCallback);
   }


private:
   void Scrub(double amount) override
   {
      SetValue(std::clamp(this->GetValue() + static_cast<N>(amount), mMin, mMax));
   }

protected:
   N mMin, mMax;
   std::function<void(N, N)> mCallback;
};

}; // namespace Editor

}; // namespace CubeWorld
