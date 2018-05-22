// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>

#include <Engine/Logger/Logger.h>
#include <Engine/GameObject/GameObject.h>

#include "State.h"

namespace CubeWorld
{

namespace Engine
{

State::State()
{
}

State::~State()
{
}

void State::AddObject(std::unique_ptr<GameObject> obj)
{
   mObjects.push_back(std::move(obj));
}

void State::Update(const Input::InputManager* input, double dt)
{
   for (const auto& object : mObjects)
   {
      if (object->mInputComponent != nullptr)
      {
         object->mInputComponent->Update(this, input, object.get());
      }
   }

   for (const auto& object : mObjects)
   {
      for (uint32_t i = 0; i < object->mNumComponents; ++i)
      {
         object->mComponents[i]->Update(this, input, dt, object.get());
      }
   }
}

void State::Render(double /*dt*/)
{
   for (const auto& object : mObjects)
   {
      object->Render(mCamera->GetPerspective(), mCamera->GetView());
   }
}

}; // namespace Engine

}; // namespace CubeWorld