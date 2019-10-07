// By Thomas Steinke

#include <algorithm>
#include <glm/ext.hpp>

#include <RGBLogger/Logger.h>
#include <Engine/Core/Config.h>

#include "../Components/VoxModel.h"
#include "Simple3DRenderSystem.h"
#include "AnimationEventSystem.h"

namespace CubeWorld
{

namespace
{

static void GetVerticesAndNormals(
   glm::vec3* outVertices,
   glm::vec3* outNormals,
   glm::mat4 matrix,
   glm::vec3 offset,
   glm::vec3 size
)
{
   outVertices[0] = matrix * glm::vec4{offset.x,          offset.y,          offset.z, 1};
   outVertices[1] = matrix * glm::vec4{offset.x,          offset.y,          offset.z + size.z, 1};
   outVertices[2] = matrix * glm::vec4{offset.x,          offset.y + size.y, offset.z, 1};
   outVertices[3] = matrix * glm::vec4{offset.x,          offset.y + size.y, offset.z + size.z, 1};
   outVertices[4] = matrix * glm::vec4{offset.x + size.x, offset.y,          offset.z, 1};
   outVertices[5] = matrix * glm::vec4{offset.x + size.x, offset.y,          offset.z + size.z, 1};
   outVertices[6] = matrix * glm::vec4{offset.x + size.x, offset.y + size.y, offset.z, 1};
   outVertices[7] = matrix * glm::vec4{offset.x + size.x, offset.y + size.y, offset.z + size.z, 1};

   outNormals[0] = glm::normalize(outVertices[1] - outVertices[0]);
   outNormals[1] = glm::normalize(outVertices[2] - outVertices[0]);
   outNormals[2] = glm::normalize(outVertices[4] - outVertices[0]);
}

}; // anonymous namespace

void AnimationEventSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{}

void AnimationEventSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   // First, update skeletons.
   entities.Each<AnimationController>([&](Engine::Entity, AnimationController& controller) {
      // Check for an un-loaded skeleton
      if (controller.skeletons.empty())
      {
         return;
      }

      for (const auto& event : controller.states[controller.current].events)
      {
         if (controller.time >= event.start && controller.time <= event.end)
         {
         }
      }
   });
}

// 
// --------------------------------------------------------
// |                                                      |
// |                     Debugging                        |
// |                                                      |
// --------------------------------------------------------
void AnimationEventDebugSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{}

void AnimationEventDebugSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
   if (!mActive)
   {
      return;
   }

   entities.Each<Engine::Transform, AnimationController, AnimationEventDebugger>([&](Engine::Transform& transform, AnimationController& controller, AnimationEventDebugger& debugger) {
      std::vector<GLfloat> points;
      std::vector<GLfloat> colors;

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
                     glm::mat4 matrix = transform.GetMatrix() * s->bones[s->boneLookup.at(event.strike.bone)].matrix;
                     Extend(points, colors, matrix, event.strike.offset - event.strike.size / glm::vec3{2}, event.strike.size);
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

      if (points.size() == 0)
      {
         return;
      }

      debugger.output->cullFaces = false;
      debugger.output->mCount = points.size();
      debugger.output->mVertices.BufferData(sizeof(GLfloat) * debugger.output->mCount, &points[0], GL_STREAM_DRAW);
      debugger.output->mColors.BufferData(sizeof(GLfloat) * debugger.output->mCount, &colors[0], GL_STREAM_DRAW);
   });
}

void AnimationEventDebugSystem::Extend(
   std::vector<GLfloat>& points,
   std::vector<GLfloat>& colors,
   glm::mat4 matrix,
   glm::vec3 offset,
   glm::vec3 size
) {
   glm::vec3 vertices[8];
   glm::vec3 normals[3];
   GetVerticesAndNormals(vertices, normals, matrix, offset, size);

   normals[0] = normals[0] * glm::vec3{20};
   normals[1] = normals[1] * glm::vec3{20};
   normals[2] = normals[2] * glm::vec3{20};

   points.insert(points.end(), {
      vertices[0].x, vertices[0].y, vertices[0].z,
      vertices[1].x, vertices[1].y, vertices[1].z,
      vertices[3].x, vertices[3].y, vertices[3].z,

      vertices[0].x, vertices[0].y, vertices[0].z,
      vertices[3].x, vertices[3].y, vertices[3].z,
      vertices[2].x, vertices[2].y, vertices[2].z,

      vertices[4].x, vertices[4].y, vertices[4].z,
      vertices[5].x, vertices[5].y, vertices[5].z,
      vertices[7].x, vertices[7].y, vertices[7].z,

      vertices[4].x, vertices[4].y, vertices[4].z,
      vertices[7].x, vertices[7].y, vertices[7].z,
      vertices[6].x, vertices[6].y, vertices[6].z,

      vertices[0].x, vertices[0].y, vertices[0].z,
      vertices[1].x, vertices[1].y, vertices[1].z,
      vertices[5].x, vertices[5].y, vertices[5].z,

      vertices[0].x, vertices[0].y, vertices[0].z,
      vertices[5].x, vertices[5].y, vertices[5].z,
      vertices[4].x, vertices[4].y, vertices[4].z,

      vertices[2].x, vertices[2].y, vertices[2].z,
      vertices[3].x, vertices[3].y, vertices[3].z,
      vertices[7].x, vertices[7].y, vertices[7].z,

      vertices[2].x, vertices[2].y, vertices[2].z,
      vertices[7].x, vertices[7].y, vertices[7].z,
      vertices[6].x, vertices[6].y, vertices[6].z,

      vertices[0].x, vertices[0].y, vertices[0].z,
      vertices[2].x, vertices[2].y, vertices[2].z,
      vertices[6].x, vertices[6].y, vertices[6].z,

      vertices[0].x, vertices[0].y, vertices[0].z,
      vertices[6].x, vertices[6].y, vertices[6].z,
      vertices[4].x, vertices[4].y, vertices[4].z,

      vertices[1].x, vertices[1].y, vertices[1].z,
      vertices[3].x, vertices[3].y, vertices[3].z,
      vertices[7].x, vertices[7].y, vertices[7].z,

      vertices[1].x, vertices[1].y, vertices[1].z,
      vertices[7].x, vertices[7].y, vertices[7].z,
      vertices[5].x, vertices[5].y, vertices[5].z,

      0, 0, 0,
      0.1, 0, 0,
      normals[0].x, normals[0].y, normals[0].z,

      0, 0, 0,
      0.1, 0, 0,
      normals[1].x, normals[1].y, normals[1].z,

      0, 0, 0,
      0.1, 0, 0,
      normals[2].x, normals[2].y, normals[2].z,
   });
   colors.insert(colors.end(), {
      1, 0, 1, 0, 0, 1, 1, 0, 0,
      1, 0, 1, 0, 0, 1, 1, 0, 0,
      1, 0, 1, 0, 0, 1, 1, 0, 0,
      1, 0, 1, 0, 0, 1, 1, 0, 0,

      1, 0, 1, 0, 0, 1, 1, 0, 0,
      1, 0, 1, 0, 0, 1, 1, 0, 0,
      1, 0, 1, 0, 0, 1, 1, 0, 0,
      1, 0, 1, 0, 0, 1, 1, 0, 0,

      1, 0, 1, 0, 0, 1, 1, 0, 0,
      1, 0, 1, 0, 0, 1, 1, 0, 0,
      1, 0, 1, 0, 0, 1, 1, 0, 0,
      1, 0, 1, 0, 0, 1, 1, 0, 0,

      0, 1, 0, 0, 1, 0, 0, 1, 0,
      0, 1, 1, 0, 1, 1, 0, 1, 1,
      1, 1, 0, 1, 1, 0, 1, 1, 0,
   });
}

}; // namespace CubeWorld
