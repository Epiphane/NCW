// By Thomas Steinke

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <Engine/Core/Maybe.h>
#include <Engine/Core/Singleton.h>
#include <Engine/Graphics/TextureAtlas.h>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

class Font {
public:
   /// Holds all state information relevant to a character as loaded using FreeType
   struct Character {
      glm::ivec2 size;    // Size of glyph
      glm::ivec2 bearing; // Offset from baseline to left/top of glyph
      int64_t advance;    // Horizontal offset to advance to next glyph
      TextureAtlas::Region region; // Region of the texture used.
   };

   struct CharacterVertexUV {
      glm::vec2 position;
      glm::vec2 uv;
   };

public:
   Font();
   Font(Font&& other);
   ~Font();

   Either<bool, std::string> Load(const FT_Library& library, const std::string& path);

   std::vector<CharacterVertexUV> Write(GLfloat x, GLfloat y, GLfloat scale, const std::string& text);

   GLuint GetTexture() { return mAtlas.GetTexture(); }

private:
   FT_Face mFace;

   TextureAtlas mAtlas;
   std::vector<Character> characters;
};

class FontManager : public Singleton<FontManager>
{
public:

public:
   FontManager();
   virtual ~FontManager() throw();

   Maybe<Font*> GetFont(const std::string& path);

   bool IsValid() { return mValid; }

private:
   FT_Library mLibrary;
   bool mValid;

   std::unordered_map<std::string, std::unique_ptr<Font>> mFonts;
};

}; // namespace Engine

}; // namespace Graphics

}; // namespace CubeWorld
