// By Thomas Steinke

#pragma once

#include <GL/includes.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

#include <RGBDesignPatterns/Maybe.h>
#include <RGBDesignPatterns/Singleton.h>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

class Texture {
public:
   Texture(/* ??? */);
   ~Texture();

   static Maybe<std::unique_ptr<Texture>> Load(const std::string& filename);

   GLuint GetTexture() { return mTexture; }
   const uint32_t GetWidth() const { return mWidth; }
   const uint32_t GetHeight() const { return mHeight; }
   const uint32_t GetDepth() const { return mDepth; }

   const glm::vec4 GetImage(const std::string& image) const;

private:
   Maybe<void> LoadPNG(const std::string& filename);

private:
   // Width (in pixels) of the texture
   uint32_t mWidth;

   // Height (in pixels) of the texture
   uint32_t mHeight;

   // Depth (in bytes) of the texture
   uint32_t mDepth;

   // OpenGL texture ID
   GLuint mTexture;

private:
   std::unordered_map<std::string, glm::vec4> mImages;
};

class TextureManager : public Singleton<TextureManager>
{
public:
   TextureManager();

   Maybe<Texture*> GetTexture(const std::string& filename);

private:
   std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
};

}; // namespace Engine

}; // namespace Graphics

}; // namespace CubeWorld
