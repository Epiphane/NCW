// By Thomas Steinke

#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <Engine/GameObject/GeneralComponent.h>

namespace CubeWorld
{

namespace Game
{

namespace Component
{

class StupidPhysicsComponent : public Engine::Component::GeneralComponent
{
public:
   struct Options {};

   StupidPhysicsComponent(const Options& options) : mOptions(options), GeneralComponent(Engine::Component::Physics) {};

   void Update(const Engine::State* world, const Engine::Input::InputManager* input, double dt, Engine::GameObject* gameObject) override;

private:
   Options mOptions;

public:
   void SetVelocity(glm::vec3 velocity) { mVelocity = velocity; }
   glm::vec3 GetVelocity() { return mVelocity; }

private:
   glm::vec3 mVelocity;
};

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
