// By Thomas Steinke

#pragma once

#include <Engine/Core/Bounded.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/UI/UIRoot.h>

#include <Shared/Components/ArmCamera.h>

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

class MainState : public Engine::State, public Engine::Receiver<MainState> {
public:
   MainState(Engine::Window* window, Bounded& parent);
   ~MainState();

   void Initialize() override;
   void SetParent(Engine::EventManager* other) { mEvents.SetParent(other); }

public:
   void Receive(const Engine::UIRebalancedEvent& evt);

private:
   Engine::Graphics::CameraHandle mCamera;
   Engine::ComponentHandle<ArmCamera> mPlayerCam;

   Engine::Window* mWindow;
   Bounded& mParent;
};

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld
