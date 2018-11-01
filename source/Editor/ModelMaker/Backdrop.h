// By Thomas Steinke

#pragma once

#include <Engine/Entity/Component.h>
#include <Engine/Event/Event.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Shared/Systems/VoxelRenderSystem.h>
#include <Shared/Components/CubeModel.h>

#include "Events.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

//
// Backdrop modifies the VoxelRender component of a backdrop object.
//
class Backdrop : public Engine::Receiver<ModelLoadedEvent> {
public:
   Backdrop(Engine::ComponentHandle<VoxelRender> component);

private:
   Voxel::Model mVoxels;
   Engine::ComponentHandle<VoxelRender> mComponent;

public:
   void Receive(const ModelLoadedEvent& evt);
};

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld
