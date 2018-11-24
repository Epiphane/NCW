// By Thomas Steinke

#pragma once

#include <Engine/Core/Bounded.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Input.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/UI/UIRoot.h>

#include <Shared/Components/ArmCamera.h>

#include "Backdrop.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

class MainState : public Engine::State, public Engine::Receiver<MainState> {
public:
   MainState(Engine::Input* input, Bounded& parent);
   ~MainState();

   void Initialize() override;
   
   void SetParent(Engine::EventManager* other) { mEvents.SetParent(other); }
   template<typename E>
   void TransformParentEvents(Engine::Transformer<E>* transformer)
   {
      mEvents.TransformParentEvents<E>(transformer);
   }

public:
   void Receive(const Engine::UIRebalancedEvent& evt);

private:
   Engine::Graphics::CameraHandle mCamera;
   Engine::ComponentHandle<ArmCamera> mPlayerCam;

   Engine::Input* mInput;
   Bounded& mParent;

private:
   std::unique_ptr<Backdrop> mBackdrop;
};

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld
