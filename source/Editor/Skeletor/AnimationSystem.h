// By Thomas Steinke

#pragma once

#include <Engine/Entity/Component.h>
#include <Engine/System/System.h>
#include <Shared/Components/DeprecatedSkeleton.h>

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

//
// A dumbed-down AnimationController, which just holds skeletons
//
struct SkeletonCollection : Engine::Component<SkeletonCollection>
{
   std::vector<Engine::ComponentHandle<DeprecatedSkeleton>> skeletons;
   std::vector<std::string> stances;
   size_t stance;
};

//
// Skeletor::AnimationSystem pretends to be like Game::AnimationSystem but
// doesn't actually animate or anything like that. It just allows for showing
// raw stance data.
//
class AnimationSystem : public Engine::System<AnimationSystem>
{
public:
   AnimationSystem() {};
   ~AnimationSystem() {}

   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
};

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld
