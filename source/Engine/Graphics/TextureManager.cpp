// By Thomas Steinke

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <glad/glad.h>
#include <lodepng/lodepng.h>

#include <Engine/Core/Format.h>
#include <Engine/Core/Paths.h>
#include <Engine/Logger/Logger.h>
#pragma warning(push, 0)
#include <Shared/Helpers/json.hpp>
#pragma warning(pop)

#include "TextureManager.h"

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

Texture::Texture()
   : mWidth(0)
   , mHeight(0)
   , mDepth(0)
   , mTexture(0)
   , mImages{}
{
   glGenTextures(1, &mTexture);
}

Texture::~Texture()
{
   glDeleteTextures(1, &mTexture);
}

Maybe<void> Texture::LoadPNG(const std::string& filename) {
   std::vector<unsigned char> data;

   //decode
   unsigned code = lodepng::decode(data, mWidth, mHeight, filename);
   if (code != 0)
   {
      return Failure(lodepng_error_text(code)).WithContext("lodepng decode failed");
   }

   glBindTexture(GL_TEXTURE_2D, mTexture);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

   //Always set reasonable texture parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//LINEAR);

   CHECK_GL_ERRORS();

   return Success;
}

Maybe<std::unique_ptr<Texture>> Texture::Load(const std::string& filename)
{
   std::unique_ptr<Texture> texture = std::make_unique<Texture>();
   Maybe<void> result = texture->LoadPNG(filename);
   if (!result)
   {
      return result.Failure().WithContext("Failed loading PNG file");
   }

   // Look for and load any metadata
   if (Paths::Exists(filename + ".json"))
   {
      std::ifstream file(filename + ".json");
      nlohmann::json metadata;
      file >> metadata;

      if (metadata["width"] != texture->mWidth)
      {
         LOG_WARNING("File %1's width of %i didn't match its metadata's width of %i", Paths::GetFilename(filename), texture->mWidth, metadata.value("width", 0));
      }
      if (metadata["height"] != texture->mHeight)
      {
         LOG_WARNING("File %1's height of %i didn't match its metadata's height of %i", Paths::GetFilename(filename), texture->mWidth, metadata.value("height", 0));
      }

      if (auto images = metadata.find("images"); images != metadata.end())
      {
         for (auto it = images->begin(); it != images->end(); it++)
         {
            nlohmann::json info = it.value();
            texture->mImages.emplace(it.key(), glm::vec4(
               info.value("x", 0.0f) / texture->mWidth,
               info.value("y", 0.0f) / texture->mHeight,
               info.value("w", 0.0f) / texture->mWidth,
               info.value("h", 0.0f) / texture->mHeight
            ));
         }
      }
   }

   return std::move(texture);
}

const glm::vec4 Texture::GetImage(const std::string& image) const
{
   auto it = mImages.find(image);
   if (it == mImages.end())
   {
      return glm::vec4(0);
   }
   return it->second;
}

TextureManager::TextureManager()
{
   mTextures.clear();
}

Maybe<Texture*> TextureManager::GetTexture(const std::string& path)
{
   // TODO normalize the path
   auto existing = mTextures.find(path);
   if (existing != mTextures.end())
   {
      return existing->second.get();
   }

   // Attempt to load the font.
   LOG_DEBUG("Loading %1", path);
   Maybe<std::unique_ptr<Texture>> newTexture = Texture::Load(path);
   if (!newTexture)
   {
      return newTexture.Failure().WithContext("Failed loading texture");
   }

   auto insertion = mTextures.emplace(path, std::move(*newTexture));
   return insertion.first->second.get();
}
   
}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
