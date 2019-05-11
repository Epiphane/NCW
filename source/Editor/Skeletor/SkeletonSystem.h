// By Thomas Steinke

#pragma once

#include <Engine/Entity/Component.h>
#include <Engine/System/System.h>
#include <Shared/Components/Skeleton.h>

namespace CubeWorld
{

namespace Editor
{

namespace Skeletor
{

//
// A dumbed-down SkeletonCollection, which just holds skeletons
//
struct SkeletonCollection : Engine::Component<SkeletonCollection>
{
public:
   void AddSkeleton(const Engine::ComponentHandle<Skeleton>& skeleton);
   void Reset();

   void SetStance(const std::string& stance);

private:
   friend class Dock;
   friend class SkeletonSystem;
   std::vector<Engine::ComponentHandle<Skeleton>> skeletons;
   std::unordered_map<std::string, std::string> parents;
   std::string stance;
   bool dirty;
};

//
// Skeletor::AnimationSystem pretends to be like Game::AnimationSystem but
// doesn't actually animate or anything like that. It just allows for showing
// raw stance data.
//
class SkeletonSystem : public Engine::System<SkeletonSystem>
{
public:
   SkeletonSystem() {};
   ~SkeletonSystem() {}

   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
};

}; // namespace Skeletor

}; // namespace Editor

}; // namespace CubeWorld
