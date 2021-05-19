// By Thomas Steinke

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include <Engine/Core/Timer.h>
#include <Engine/Entity/EntityManager.h>
#include <Engine/Graphics/Camera.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/System/System.h>

#include "../DebugHelper.h"

namespace CubeWorld
{

constexpr GLuint kPrimitiveRestart = GLuint(-1);

struct Simple3DRender : public Engine::Component<Simple3DRender> {
    Simple3DRender(std::vector<GLfloat>&& points, std::vector<GLfloat>&& colors);
    Simple3DRender(std::vector<glm::vec3>&& points, std::vector<glm::vec3>&& colors);
    Simple3DRender(const Simple3DRender& other);
   
    Engine::Graphics::VBO mVertices, mColors;
    size_t mCount = 0;
    bool cullFaces = true;
    GLuint renderType = GL_TRIANGLES;
};

struct Index3DRender : public Engine::Component<Index3DRender> {
    Index3DRender(std::vector<glm::vec3>&& points, std::vector<glm::vec3>&& colors, std::vector<GLuint>&& indices, std::vector<glm::vec3>&& positions);
    Index3DRender(const Index3DRender& other);

    Engine::Graphics::VBO mVertices, mColors, mIndices, mOffsets;
    size_t mCount = 0;
    size_t mInstances = 0;
    GLuint renderType = GL_TRIANGLES;
};

struct ShadedMesh : public Engine::Component<ShadedMesh> {
    void Set(
        Engine::Graphics::VBO&& mVertices,
        Engine::Graphics::VBO&& mColors,
        Engine::Graphics::VBO&& mNormals,
        Engine::Graphics::VBO&& mIndices,
        size_t vertexCount,
        size_t indexCount
    );

    Engine::Graphics::VBO mVertices;
    Engine::Graphics::VBO mColors;
    Engine::Graphics::VBO mNormals;
    Engine::Graphics::VBO mIndices;
    size_t mVertexCount = 0;
    size_t mIndexCount = 0;
    GLuint renderType = GL_TRIANGLES;
};

class Simple3DRenderSystem : public Engine::System<Simple3DRenderSystem> {
public:
    Simple3DRenderSystem(Engine::Graphics::Camera* camera = nullptr);
    ~Simple3DRenderSystem();

    void Configure(Engine::EntityManager& entities, Engine::EventManager& events) override;

    void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

    void SetCamera(Engine::Graphics::Camera* camera) { mCamera = camera; }
   
private:
    Engine::Graphics::Camera* mCamera;

    static std::unique_ptr<Engine::Graphics::Program> stupid;
    static std::unique_ptr<Engine::Graphics::Program> shaded;
};

}; // namespace CubeWorld
