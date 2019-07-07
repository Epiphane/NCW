// By Thomas Steinke

#pragma once

#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Input.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/UI/UIRoot.h>

#include <Shared/Components/ArmCamera.h>

#include "Events.h"

namespace CubeWorld
{

namespace Editor
{

namespace ParticleSpace
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
   void Receive(const ClearParticleEmitterEvent& evt);
   void Receive(const LoadParticleEmitterEvent& evt);

private:
   Engine::Graphics::CameraHandle mCamera;
   Engine::ComponentHandle<ArmCamera> mPlayerCam;

   Engine::Entity mParticleSpawner;
   Engine::Input* mInput;
   Bounded& mParent;
};

}; // namespace ParticleSpace

}; // namespace Editor

}; // namespace CubeWorld
