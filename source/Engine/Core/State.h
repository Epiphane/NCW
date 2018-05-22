// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/Input.h>
#include <Engine/Graphics/Camera.h>

namespace CubeWorld
{

namespace Engine
{

class GameObject;

class State {
private:
   std::vector<std::unique_ptr<GameObject>> mObjects;
   std::vector<GameObject*> mRendererQueue;

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
   virtual void Update(const Input::InputManager* input, double dt);
   virtual void Render(double dt);
   //virtual void regen_resources();

   //virtual void send(std::string message, void *data);

   void AddObject(std::unique_ptr<GameObject> obj);
};

}; // namespace Engine

}; // namespace CubeWorld
