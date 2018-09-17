// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Shared/Event/NamedEvent.h>
#include <Shared/Systems/AnimationSystem.h>

#include "../UI/Controls.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

class MainState : public Engine::State, public Engine::Receiver<MainState> {
public:
   MainState(Engine::Window* window, Bounded& parent);
   ~MainState();

   void Initialize() override;

   Engine::ComponentHandle<Game::AnimatedSkeleton> GetPlayerSkeleton()
   {
      return mPlayer.Get<Game::AnimatedSkeleton>();
   }

private:
   Engine::Graphics::CameraHandle mCamera;

   Engine::Window* mWindow;
   Bounded& mParent;

   Engine::Entity mPlayer;
   std::string mFilename;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld
