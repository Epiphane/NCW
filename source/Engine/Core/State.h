// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/Config.h>
#include <Engine/Core/Input.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/System/SystemManager.h>
#include <Engine/Graphics/Camera.h>

namespace CubeWorld
{

namespace Engine
{

class GameObject;

class State {
private:
   //void UpdateRendererQueue();

protected:
   Graphics::Camera* mCamera;

public:
   State();
   ~State();

   bool initialized = false;
   virtual void Start() = 0;
   //virtual void pause();
   //virtual void unpause();
   virtual void Update(TIMEDELTA dt);
   //virtual void regen_resources();

   //virtual void send(std::string message, void *data);

   //void AddObject(std::unique_ptr<GameObject> obj);

protected:
   // EventManager events;
   EntityManager mEntities;
   SystemManager mSystems;
};

}; // namespace Engine

}; // namespace CubeWorld
