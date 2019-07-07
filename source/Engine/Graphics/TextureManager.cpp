// By Thomas Steinke

#include <algorithm>
#include <cstdlib>
#include <glad/glad.h>
#include <lodepng/lodepng.h>
#include <random>

#include <RGBText/Format.h>
#include <RGBFileSystem/FileSystem.h>
#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/JSONSerializer.h>
#include <RGBNetworking/YAMLSerializer.h>

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
      return Failure{lodepng_error_text(code)}.WithContext("lodepng decode failed");
   }

   glBindTexture(GL_TEXTURE_2D, mTexture);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)mWidth, (GLsizei)mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

   //Always set reasonable texture parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//LINEAR);

   CHECK_GL_ERRORS();

   return Success;
}

Maybe<void> Texture::LoadRandom(uint32_t size)
{
   std::vector<glm::vec3> data;
   data.resize(size);

   std::default_random_engine generator;
   std::uniform_real_distribution<float> distribution(-1, 1);
   for (unsigned int i = 0 ; i < size ; i++) {
      data[i].x = distribution(generator);
      data[i].y = distribution(generator);
      data[i].z = distribution(generator);
   }
   
   glBindTexture(GL_TEXTURE_1D, mTexture);
   glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, (GLsizei)size, 0, GL_RGB, GL_FLOAT, data.data());
   glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   
   GLenum err = glGetError();
   if (err != GL_NO_ERROR)
   {
      return Failure{"GL error: %1", err};
   }

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

   BindingProperty metadata;

   // Look for and load any metadata
#pragma warning(disable : 4101)
   if (auto[_, exists] = DiskFileSystem{}.Exists(filename + ".yaml"); exists)
#pragma warning(default : 4101)
   {
      Maybe<BindingProperty> maybeMetadata = YAMLSerializer::DeserializeFile(filename + ".yaml");
      if (!maybeMetadata)
      {
         return maybeMetadata.Failure().WithContext("Failed reading metadata");
      }
      else
      {
         metadata = std::move(*maybeMetadata);
      }
   }
#pragma warning(disable : 4101 4456 6246)
   else if (auto[_, exists] = DiskFileSystem{}.Exists(filename + ".json"); exists)
#pragma warning(default : 4101 4456 6246)
   {
      Maybe<BindingProperty> maybeMetadata = JSONSerializer::DeserializeFile(filename + ".json");
      if (!maybeMetadata)
      {
         return maybeMetadata.Failure().WithContext("Failed reading metadata");
      }
      else
      {
         metadata = std::move(*maybeMetadata);
      }
   }

   if (metadata["width"] != texture->mWidth)
   {
      LOG_WARNING("File %1's width of %i didn't match its metadata's width of %i", Paths::GetFilename(filename), texture->mWidth, metadata["width"].GetUintValue());
   }
   if (metadata["height"] != texture->mHeight)
   {
      LOG_WARNING("File %1's height of %i didn't match its metadata's height of %i", Paths::GetFilename(filename), texture->mWidth, metadata["height"].GetUintValue());
   }

   for (const auto [name, info] : metadata["images"].pairs())
   {
      texture->mImages.emplace(name, glm::vec4(
         info["x"].GetFloatValue() / texture->mWidth,
         info["y"].GetFloatValue() / texture->mHeight,
         info["w"].GetFloatValue() / texture->mWidth,
         info["h"].GetFloatValue() / texture->mHeight
      ));
   }

   return std::move(texture);
}

Maybe<std::unique_ptr<Texture>> Texture::MakeRandom(uint32_t size)
{
   std::unique_ptr<Texture> texture = std::make_unique<Texture>();
   Maybe<void> result = texture->LoadRandom(size);
   if (!result)
   {
      return result.Failure().WithContext("Failed generating random texture");
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
