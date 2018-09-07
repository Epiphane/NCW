// By Thomas Steinke

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <forward_list>

#include <Engine/Core/Maybe.h>
#include <Engine/Core/Singleton.h>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{
   
   //
   // TextureAtlas provides a method for packing lots of small images into
   // a single texture. This is useful for asset packing, especially with
   // loading fonts into a single texture that can then be referenced by coordinates.
   //
   // The actual implementation is based on the article by Jukka Jylänki : "A
   // Thousand Ways to Pack the Bin - A Practical Approach to Two-Dimensional
   // Rectangle Bin Packing", February 27, 2010.
   //
   // Specifically, this implements the Skyline Bottom-Left
   // algorithm based on C++ sources provided by Jukka Jylänki at:
   // http://clb.demon.fi/files/RectangleBinPack/
   //
   // Example usage:
   //
   // ```
   // TextureAtlas atlas(512, 512, 1);
   //
   // Maybe<TextureAtlas::Region> region = atlas.Allocate(20, 20);
   // if (region) {
   //   atlas.Fill(region, mydata, stride);  
   // }
   // ```
   class TextureAtlas {
   public:
      struct Region
      {
         GLsizei x, y;
         GLsizei w, h;
      };

   public:
      TextureAtlas(GLsizei width, GLsizei height, size_t depth = 1);
      TextureAtlas(TextureAtlas&& other);
      ~TextureAtlas();

      Maybe<TextureAtlas::Region> Allocate(GLsizei width, GLsizei height);
      void Fill(Region region, const void* data, const size_t stride);

      // GetTexture updates or creates a texture for its data if necessary, then returns the ID.
      GLuint GetTexture();

      const GLsizei GetWidth() const { return mWidth; }
      const GLsizei GetHeight() const { return mHeight; }

   private:
      // Allocated regions. Height is always equal to (atlas height) - (node y),
      // but the memory savings are negligible so we just reuse the struct.
      //
      // This list is naturally sorted by node.x, with no duplicates in that position.
      std::forward_list<Region> mNodes;
   
      // Width (in pixels) of the texture
      GLsizei mWidth;

      // Height (in pixels) of the texture
      GLsizei mHeight;

      // Depth (in bytes) of the texture
      size_t mDepth;

      // Allocated space in the current texture.
      size_t mUsed;

      // OpenGL texture ID
      GLuint mTexture;

      // Texture dirty bit
      bool mDirty;

      // Atlas data
      uint8_t* mData;
   };

}; // namespace Engine

}; // namespace Graphics

}; // namespace CubeWorld
