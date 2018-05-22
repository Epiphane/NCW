// By Thomas Steinke

#include <Engine/Logger/Logger.h>

#include "GameObject.h"

namespace CubeWorld
{

namespace Engine
{

GameObject::GameObject(Options& options)
   : mRemove(false)
   , mName(options.name)
   , mLayerMask(options.layerMask)
   , mPosition(glm::vec3(0, 0, 0))
   , mScale(glm::vec3(1, 1, 1))
   , mRotation(glm::vec3(0, 0, 0))
{
   mInputComponent = std::move(options.inputComponent);
   mGraphicsComponent = std::move(options.graphicsComponent);

   mNumComponents = options.numComponents;
   for (int i = 0; i < mNumComponents; ++i)
   {
      mComponents[i] = std::move(options.components[i]);
   }

   mModel = glm::mat4{1};
}

GameObject::~GameObject() {}

void GameObject::Render(
   const glm::mat4& perspective,
   const glm::mat4& view
)
{
   if (mGraphicsComponent != nullptr)
   {
      mGraphicsComponent->Render(perspective, view, mModel, this);
   }
}

std::pair<int16_t, Component::GeneralComponent*> GameObject::GetNextComponentOfType(Component::ComponentType type, int16_t lastIndex)
{
   while (++lastIndex < mNumComponents)
   {
      if (mComponents[lastIndex]->mType & type)
      {
         return std::make_pair(lastIndex, mComponents[lastIndex].get());
      }
   }

   return std::make_pair(mNumComponents, nullptr);
}

}; // namespace Engine

}; // namespace CubeWorld