// By Thomas Steinke

#pragma once

#include <Engine/Event/Event.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Shared/Components/CubeModel.h>

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

struct ModelLoadedEvent : public Engine::Event<ModelLoadedEvent>
{
   ModelLoadedEvent(Engine::ComponentHandle<CubeModel> component) : component(component) {};

   Engine::ComponentHandle<CubeModel> component;
};

struct ModelSavedEvent : public Engine::Event<ModelSavedEvent>
{
   ModelSavedEvent(Engine::ComponentHandle<CubeModel> component) : component(component) {};

   Engine::ComponentHandle<CubeModel> component;
};

struct ModelModifiedEvent : public Engine::Event<ModelModifiedEvent>
{
   ModelModifiedEvent(Engine::ComponentHandle<CubeModel> component) : component(component) {};

   Engine::ComponentHandle<CubeModel> component;
};

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld
