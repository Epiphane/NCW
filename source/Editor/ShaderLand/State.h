// By Thomas Steinke

#pragma once

#include <vector>

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

namespace ShaderLand
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
    // Event processing

private:
   Engine::Graphics::CameraHandle mCamera;
   Engine::ComponentHandle<ArmCamera> mPlayerCam;

   Engine::Entity mMesh;
   Engine::Input* mInput;
   Bounded& mParent;
};

}; // namespace ShaderLand

}; // namespace Editor

}; // namespace CubeWorld
