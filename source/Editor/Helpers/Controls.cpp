// By Thomas Steinke

#include <cassert>
#include <functional>
#include <noise/noise.h>
#include <noiseutils/noiseutils.h>

#include <Engine/Core/StateManager.h>
#include <Engine/Logger/Logger.h>
#include <Engine/Entity/Transform.h>
#include <Engine/System/InputEventSystem.h>
#include <Shared/Components/CubeModel.h>
#include <Shared/Components/ArmCamera.h>
#include <Shared/Systems/AnimationSystem.h>
#include <Shared/Systems/CameraSystem.h>
#include <Shared/Systems/FlySystem.h>
#include <Shared/Systems/MakeshiftSystem.h>
#include <Shared/Systems/Simple3DRenderSystem.h>
#include <Shared/Systems/SimplePhysicsSystem.h>
#include <Shared/Systems/VoxelRenderSystem.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "Controls.h"

namespace CubeWorld
{

namespace Editor
{

Controls::Controls(Engine::Window* window)
   : mWindow(window)
{
}

Controls::~Controls()
{
}

void Controls::Update()
{
   // Render
}

void Controls::Rebuild(const Layout& layout)
{

}

}; // namespace Editor

}; // namespace CubeWorld
