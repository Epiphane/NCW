// By Thomas Steinke

#include "../Core/Window.h"
#include "../Logger/Logger.h"

#include "Image.h"

namespace CubeWorld
{

namespace Engine
{

namespace Aggregator
{

std::unique_ptr<Engine::Graphics::Program> Image::program = nullptr;

Image::Image()
{
   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load("Shaders/2DTexture.vert", "Shaders/2DTexture.geom", "Shaders/2DTexture.frag");
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading 2D Texture shader").GetMessage());
         return;
      }

      program = std::move(*maybeProgram);

      program->Attrib("aPosition");
      program->Attrib("aUV");
      program->Uniform("uTexture");
      program->Uniform("uWindowSize");
   }
}

void Image::ConnectToTexture(const Region& region, GLuint texture)
{
   auto entry = mTextureIndices.find(texture);
   if (entry == mTextureIndices.end())
   {
      VBOWithData data = std::make_pair(Graphics::VBO(Graphics::VBO::Indices), std::vector<GLuint>());
      mTextureIndices.emplace(texture, std::move(data));
   }

   VBOWithData& vboData = mTextureIndices[texture];
   for (size_t index = region.index(); index < region.index() + region.size(); index++)
   {
      vboData.second.push_back(static_cast<GLuint>(index));
   }
   vboData.first.BufferData(GLuint(sizeof(GLuint) * vboData.second.size()), vboData.second.data(), GL_STATIC_DRAW);
}

void Image::Render()
{
   Window* pWindow = Window::Instance();

   BIND_PROGRAM_IN_SCOPE(program);

   for (auto& pair : mTextureIndices)
   {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, pair.first);
      program->Uniform1i("uTexture", 0);
      program->Uniform2f("uWindowSize", static_cast<GLfloat>(pWindow->GetWidth()), static_cast<GLfloat>(pWindow->GetHeight()));

      mVBO.AttribPointer(program->Attrib("aPosition"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)0);
      mVBO.AttribPointer(program->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(float) * 2));

      VBOWithData indices = pair.second;
      indices.first.Bind();
      glDrawElements(GL_LINES, GLuint(indices.second.size()), GL_UNSIGNED_INT, (void*)0);
   }
}

}; // namespace Aggregator
   
}; // namespace Engine

}; // namespace CubeWorld
