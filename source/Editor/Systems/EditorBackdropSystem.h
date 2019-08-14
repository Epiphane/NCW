// By Thomas Steinke

#pragma once

#include <Engine/Entity/Component.h>
#include <Engine/Graphics/Program.h>
#include <Engine/System/System.h>
#include <Shared/Systems/VoxelRenderSystem.h>

namespace CubeWorld
{

namespace Editor
{

//
// Adds a wall to the bottom of the editor, of the specified color.
//
Maybe<void> AddFloor(Engine::EntityManager &entities, glm::vec3 color);

//
// Adds an origin-based wireframe to the editor, of the specified color.
//
struct EditorWireframe : public Engine::Component<EditorWireframe>
{
   glm::vec3 color = {0.6, 0.6, 0.6};
};

class EditorBackdropSystem : public Engine::System<EditorBackdropSystem> {
public:
   EditorBackdropSystem(Engine::Graphics::Camera* camera);
   ~EditorBackdropSystem();

   void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
   
private:
   Engine::Graphics::Camera* mCamera;
   Engine::Graphics::VBO mVBO;

   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace Editor

}; // namespace CubeWorld
