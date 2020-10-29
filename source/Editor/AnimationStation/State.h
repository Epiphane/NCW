// By Thomas Steinke

#pragma once

#include <Engine/Core/Bounded.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Input.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>

#include <Shared/Components/ArmCamera.h>

#include "Events.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
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
   void Receive(const SkeletonClearedEvent& evt);
   void Receive(const AddSkeletonPartEvent& evt);

private:
   Engine::Graphics::CameraHandle mCamera;
   Engine::ComponentHandle<ArmCamera> mPlayerCam;

   Engine::Entity mPlayer;
   std::vector<Engine::Entity> mPlayerParts;

   Engine::Input* mInput;
   Bounded& mParent;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld
