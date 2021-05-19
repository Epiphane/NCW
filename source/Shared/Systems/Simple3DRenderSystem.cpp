// By Thomas Steinke

#include <iostream>
#include <cassert>
#include <functional>
#include <mutex>

#include <glad/glad.h>
#include <glm/ext.hpp>

#include <RGBDesignPatterns/Scope.h>
#include <Engine/Core/Timer.h>
#include <Engine/Graphics/Program.h>
#include <Shared/Helpers/Asset.h>
#include <RGBLogger/Logger.h>

#include "Simple3DRenderSystem.h"

namespace CubeWorld
{

std::mutex gSimple3DMutex;

Simple3DRender::Simple3DRender()
    : mVertices(Engine::Graphics::VBO::Vertices)
    , mColors(Engine::Graphics::VBO::Colors)
    , mCount(0)
{}

Simple3DRender::Simple3DRender(std::vector<GLfloat>&& points, std::vector<GLfloat>&& colors)
    : Simple3DRender()
{
   mCount = points.size() / 3;
   mVertices.BufferData(sizeof(GLfloat) * mCount * 3, &points[0], GL_STATIC_DRAW);
   mColors.BufferData(sizeof(GLfloat) * mCount * 3, &colors[0], GL_STATIC_DRAW);
}

Simple3DRender::Simple3DRender(std::vector<glm::vec3>&& points, std::vector<glm::vec3>&& colors)
    : Simple3DRender()
{
    mCount = points.size();
    mVertices.BufferData(sizeof(glm::vec3) * mCount, &points[0], GL_STATIC_DRAW);
    mColors.BufferData(sizeof(glm::vec3) * mCount, &colors[0], GL_STATIC_DRAW);
}

Simple3DRender::Simple3DRender(const Simple3DRender& other)
    : mVertices(other.mVertices)
    , mColors(other.mColors)
    , mCount(other.mCount)
{}

Index3DRender::Index3DRender()
    : mVertices(Engine::Graphics::VBO::Vertices)
    , mColors(Engine::Graphics::VBO::Colors)
    , mIndices(Engine::Graphics::VBO::Indices)
    , mOffsets(Engine::Graphics::VBO::Vertices)
    , mCount(0)
{}

Index3DRender::Index3DRender(std::vector<glm::vec3>&& points, std::vector<glm::vec3>&& colors, std::vector<GLuint>&& indices, std::vector<glm::vec3>&& positions)
    : Index3DRender()
{
    mCount = indices.size();
    mVertices.BufferData(sizeof(glm::vec3) * mCount, &points[0], GL_STATIC_DRAW);
    mColors.BufferData(sizeof(glm::vec3) * mCount, &colors[0], GL_STATIC_DRAW);
    mIndices.BufferData(sizeof(GLuint) * mCount, &indices[0], GL_STATIC_DRAW);

    mInstances = positions.size();
    mOffsets.BufferData(sizeof(glm::vec3) * mInstances, &positions[0], GL_STATIC_DRAW);
}

Index3DRender::Index3DRender(const Index3DRender& other)
    : mVertices(other.mVertices)
    , mColors(other.mColors)
    , mIndices(other.mIndices)
    , mOffsets(other.mOffsets)
    , mCount(other.mCount)
    , renderType(other.renderType)
{}

ShadedMesh::ShadedMesh()
    : mVertices(Engine::Graphics::VBO::Vertices)
    , mColors(Engine::Graphics::VBO::Colors)
    , mNormals(Engine::Graphics::VBO::Normals)
    , mIndices(Engine::Graphics::VBO::Indices)
    , mVertexCount(0)
    , mIndexCount(0)
{}

void ShadedMesh::Set(
    Engine::Graphics::VBO&& vertices,
    Engine::Graphics::VBO&& colors,
    Engine::Graphics::VBO&& normals,
    Engine::Graphics::VBO&& indices,
    size_t vertexCount,
    size_t indexCount
)
{
    std::unique_lock<std::mutex> lock{gSimple3DMutex};
    mVertices = std::move(vertices);
    mColors = std::move(colors);
    mNormals = std::move(normals);
    mIndices = std::move(indices);
    mVertexCount = vertexCount;
    mIndexCount = indexCount;
}

std::unique_ptr<Engine::Graphics::Program> Simple3DRenderSystem::stupid = nullptr;
std::unique_ptr<Engine::Graphics::Program> Simple3DRenderSystem::shaded = nullptr;

Simple3DRenderSystem::Simple3DRenderSystem(Engine::Graphics::Camera* camera) : mCamera(camera)
{
}

Simple3DRenderSystem::~Simple3DRenderSystem()
{
}

void Simple3DRenderSystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{
    if (!stupid)
    {
        auto maybeProgram = Engine::Graphics::Program::Load(Asset::Shader("Stupid.vert"), Asset::Shader("Stupid.frag"));
        if (!maybeProgram)
        {
            LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading Stupid shader").GetMessage());
            return;
        }

        stupid = std::move(*maybeProgram);
        stupid->Attrib("aPosition");
        stupid->Attrib("aColor");
        stupid->Uniform("uProjMatrix");
        stupid->Uniform("uViewMatrix");
        stupid->Uniform("uModelMatrix");
    }

    if (!shaded)
    {
        auto maybeProgram = Engine::Graphics::Program::Load(Asset::Shader("ShadedMesh.vert"), Asset::Shader("ShadedMesh.frag"));
        if (!maybeProgram)
        {
            LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading ShadedMesh shader").GetMessage());
            return;
        }

        shaded = std::move(*maybeProgram);
        shaded->Attrib("aPosition");
        shaded->Attrib("aColor");
        shaded->Attrib("aNormal");
        shaded->Attrib("aOcclusion");
        shaded->Uniform("uProjMatrix");
        shaded->Uniform("uViewMatrix");
        shaded->Uniform("uModelMatrix");
    }
}

using Transform = Engine::Transform;

template<typename T, typename U> constexpr void* offsetOf(U T::* member)
{
    return &((T*)nullptr->*member);
}

void Simple3DRenderSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
    glm::mat4 perspective = mCamera->GetPerspective();
    glm::mat4 view = mCamera->GetView();

    std::unique_lock<std::mutex> lock{gSimple3DMutex};
    {
        BIND_PROGRAM_IN_SCOPE(stupid);
        stupid->UniformMatrix4f("uProjMatrix", perspective);
        stupid->UniformMatrix4f("uViewMatrix", view);

        entities.Each<Transform, Simple3DRender>([&](Transform& transform, Simple3DRender& render) {
            if (render.mCount == 0)
            {
                return;
            }

            if (!render.cullFaces)
            {
                glDisable(GL_CULL_FACE);
            }

            render.mVertices.AttribPointer(stupid->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
            render.mColors.AttribPointer(stupid->Attrib("aColor"), 3, GL_FLOAT, GL_FALSE, 0, 0);

            glm::mat4 model = transform.GetMatrix();
            stupid->UniformMatrix4f("uModelMatrix", model);

            glDrawArrays(render.renderType, 0, GLsizei(render.mCount));
            glEnable(GL_CULL_FACE);

            CHECK_GL_ERRORS();
        });

        /*
        GLuint PRIMITIVE_RESTART = 12345; // magic value
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(PRIMITIVE_RESTART);
        */

        entities.Each<Transform, Index3DRender>([&](Transform& transform, Index3DRender& render) {
            if (render.mCount == 0)
            {
                return;
            }

            stupid->UniformMatrix4f("uModelMatrix", transform.GetMatrix());

            render.mVertices.AttribPointer(stupid->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
            render.mColors.AttribPointer(stupid->Attrib("aColor"), 3, GL_FLOAT, GL_FALSE, 0, 0);
            render.mOffsets.AttribPointer(stupid->Attrib("aOffset"), 3, GL_FLOAT, GL_FALSE, 0, 0);
            glVertexAttribDivisor(stupid->Attrib("aOffset"), 1);

            render.mIndices.Bind();
            glDrawElementsInstanced(render.renderType, GLsizei(render.mCount), GL_UNSIGNED_INT, nullptr, GLsizei(render.mInstances));

            CHECK_GL_ERRORS();
        });
    }

    {
        BIND_PROGRAM_IN_SCOPE(shaded);
        shaded->UniformMatrix4f("uProjMatrix", perspective);
        shaded->UniformMatrix4f("uViewMatrix", view);

        entities.Each<Transform, ShadedMesh>([&](Transform& transform, ShadedMesh& mesh) {
            if (mesh.mIndexCount == 0)
            {
                return;
            }

            shaded->UniformMatrix4f("uModelMatrix", transform.GetMatrix());
            CHECK_GL_ERRORS();

            auto loc = shaded->Attrib("aPosition");
            glEnableVertexAttribArray(loc);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.mVertices.GetBuffer());
            glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0);

            loc = shaded->Attrib("aColor");
            glEnableVertexAttribArray(loc);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.mColors.GetBuffer());
            glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0);

            loc = shaded->Attrib("aNormal");
            glEnableVertexAttribArray(loc);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.mNormals.GetBuffer());
            glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0);

            /*
            mesh.mVertices.AttribPointer(shaded->Attrib("aPosition"), 4, GL_FLOAT, GL_FALSE, 0, 0);
            mesh.mColors.AttribPointer(shaded->Attrib("aColor"), 4, GL_FLOAT, GL_FALSE, 0, 0);
            mesh.mNormals.AttribPointer(shaded->Attrib("aNormal"), 4, GL_FLOAT, GL_FALSE, 0, 0);
            */
            //mesh.mVertices.AttribPointer(shaded->Attrib("aOcclusion"), 1, GL_FLOAT, GL_FALSE, sizeof(), offsetOf(&ShadedMesh::Point::occlusion));

            CHECK_GL_ERRORS();

            //mesh.mIndices.Bind();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mIndices.GetBuffer());
            CHECK_GL_ERRORS();
            glDrawElements(mesh.renderType, GLsizei(mesh.mIndexCount), GL_UNSIGNED_INT, (void*)0);
            CHECK_GL_ERRORS();
        });
    }
}

}; // namespace CubeWorld
