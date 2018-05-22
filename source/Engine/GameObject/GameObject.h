// By Thomas Steinke

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <Engine/Core/Either.h>
#include <Engine/GameObject/InputComponent.h>
#include <Engine/GameObject/GeneralComponent.h>
#include <Engine/GameObject/GraphicsComponent.h>

namespace CubeWorld
{

namespace Engine
{

class GameObject {
private:
   const static uint32_t MAX_COMPONENTS = 8;

   // Remove this game object
   bool mRemove;

   // Identity
   std::string mName;
   uint64_t mLayerMask;

   // Positional information
   glm::vec3 mPosition, mScale, mRotation;
   glm::mat4 mModel;

public:
   // Components can be accessed and independently called by State.
   Pointer<Component::InputComponent> mInputComponent;
   Pointer<Component::GraphicsComponent> mGraphicsComponent;
   Pointer<Component::GeneralComponent> mComponents[MAX_COMPONENTS];
   uint32_t mNumComponents;

public:
   struct Options {
      std::string name = "";
      uint64_t layerMask = 0;

      Pointer<Component::InputComponent> inputComponent = nullptr;
      Pointer<Component::GraphicsComponent> graphicsComponent = nullptr;
      Pointer<Component::GeneralComponent> components[MAX_COMPONENTS];
      uint32_t numComponents = 0;
   };

   GameObject(Options& options);
   ~GameObject();

   bool IsDead() { return mRemove; }
   void Remove() { mRemove = true; }

   glm::vec3 GetPosition() { return mPosition; }
   glm::vec3 GetRotation() { return mRotation; }
   glm::vec3 GetScale() { return mScale; }

   void SetPosition(glm::vec3 position) { mPosition = position; }
   void SetRotation(glm::vec3 rotation) { mRotation = rotation; }
   void SetScale(glm::vec3 scale) { mScale = scale; }
   
   void Render(
      const glm::mat4& perspective,
      const glm::mat4& view
   );

   std::pair<int16_t, Component::GeneralComponent*> GetNextComponentOfType(Component::ComponentType type, int16_t lastIndex = -1);
};

}; // namespace Engine

}; // namespace CubeWorld
