// By Thomas Steinke

#include <algorithm>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/ext.hpp>
#include <optional>

#include <RGBLogger/Logger.h>
#include <Engine/Core/Config.h>

#include "../Components/VoxModel.h"
#include "Simple3DRenderSystem.h"
#include "AnimationEventSystem.h"

namespace CubeWorld
{

void AnimationEventSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{}

void AnimationEventSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   // Erase stale objects from the world
   for (const auto& stale : mStaleObjects)
   {
      mPhysics.GetWorld()->removeCollisionObject(stale.ghost.get());
   }
   mStaleObjects.clear();

   // Now go through all animation events and update them
   entities.Each<Engine::Transform, AnimationController>([&](Engine::Entity entity, Engine::Transform& transform, AnimationController& controller) {
      // Check for an un-loaded skeleton
      if (controller.skeletons.empty())
      {
         return;
      }

      const uint32_t index = entity.GetID().index();

      for (const auto& event : controller.states[controller.current].events)
      {
         if (controller.time >= event.start && controller.time <= event.end)
         {
            switch (event.type)
            {
            case SkeletonAnimations::Event::Type::Strike:
            {
               for (const auto& s : controller.skeletons)
               {
                  if (s->boneLookup.count(event.strike.bone) != 0)
                  {
                     glm::mat4 matrix = transform.GetMatrix() * s->bones[s->boneLookup.at(event.strike.bone)].matrix * glm::translate(glm::mat4{1}, event.strike.offset);

                     btTransform bulletTransform;
                     bulletTransform.setFromOpenGLMatrix(glm::value_ptr(matrix));

                     btVector3 dimensions;
                     dimensions.setX(event.strike.size.x / 2);
                     dimensions.setY(event.strike.size.y / 2);
                     dimensions.setZ(event.strike.size.z / 2);
                     std::unique_ptr<btCollisionShape> shape{new btBoxShape(dimensions)};

                     std::unique_ptr<btGhostObject> ghost{new btPairCachingGhostObject()};
                     ghost->setWorldTransform(bulletTransform);
                     ghost->setCollisionShape(shape.get());
                     ghost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

                     mPhysics->GetWorld()->addCollisionObject(ghost.get(), btBroadphaseProxy::StaticFilter, btBroadphaseProxy::CharacterFilter);

                     btAlignedObjectArray<btCollisionObject*>& overlappingObjects = ghost->getOverlappingPairs();
                     const int numObjects = overlappingObjects.size();
                     for (int i = 0; i < numObjects; i++)
                     {
                        btCollisionObject* colObj = overlappingObjects[i];
                        uint32_t otherIndex = (uint32_t)colObj->getUserIndex();

                        if (otherIndex == index)
                        {
                           continue;
                        }

                        LOG_DEBUG("AnimationEventSystem: %1 struck %2!", index, otherIndex);
                     }

                     mStaleObjects.push_back(AnimationEventStaleObject{
                        std::move(shape),
                        std::move(ghost)
                     });
                     break;
                  }
               }

               break;
            }

            default:
               // Nothing to do
               break;
            }
         }
      }
   });
}

}; // namespace CubeWorld
