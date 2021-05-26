// By Thomas Steinke

#pragma once

#include <string>

#include <Engine/UI/UIRoot.h>

#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ShaderLand
{

class Mesh : public Engine::UIElement {
public:
    Mesh(Engine::UIRoot* root, Engine::UIElement* parent);

   void Update(TIMEDELTA dt) override;

private:
   // Actions
   void LoadNewFile();
   void SaveNewFile();

   BindingProperty Serialize();
   void SaveFile();
   void LoadFile(const std::string& filename);

   void UpdateMesh();

public:
   // Event handlers
    void Receive(const Engine::ComponentAddedEvent<ShadedMesh>& evt);
    void Receive(const Engine::ComponentRemovedEvent<ShadedMesh>& evt);

private:
   // State
   std::string mFilename;
   BindingProperty mSaveData;

   Engine::ComponentHandle<ShadedMesh> mMesh;
   GLuint mRenderType = GL_TRIANGLES;
   std::vector<glm::vec4> mVertices;
   std::vector<glm::vec4> mColors;
   std::vector<glm::vec4> mNormals;
   std::vector<int32_t> mIndices;
};

}; // namespace ShaderLand

}; // namespace Editor

}; // namespace CubeWorld
