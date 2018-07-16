// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include "Config.h"
#include "Input.h"
#include "../Entity/EntityManager.h"
#include "../Event/EventManager.h"
#include "../System/SystemManager.h"
#include "../Graphics/Camera.h"

namespace CubeWorld
{

namespace Engine
{

class GameObject;

class State {
private:
   //void UpdateRendererQueue();

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
   EventManager mEvents;
   EntityManager mEntities;
   SystemManager mSystems;
};

}; // namespace Engine

}; // namespace CubeWorld
