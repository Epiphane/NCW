// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include "FontManager.h"

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{
   
Font::Font() 
   : mFace(nullptr)
   , mAtlas(512, 512)
{
}

Font::Font(Font&& other) 
   : mAtlas(std::move(other.mAtlas))
{
   mFace = other.mFace;
   if (FT_Reference_Face(mFace))
   {
      LOG_ERROR("Failed to add a reference to font face");
   }
}

Maybe<void> Font::Load(const FT_Library& library, const std::string& path)
{
   assert(!mFace);
   if (FT_New_Face(library, path.c_str(), 0, &mFace))
   {
      return Failure{"Failed to load font: " + path};
   }

   if (FT_Set_Pixel_Sizes(mFace, 0, 32))
   {
      return Failure{"Failed to set pixel size to 32"};
   }

   // Disable byte-alignment restriction
   // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   // Load the 128 ASCII characters
   for (GLubyte c = 0; c < 128; ++c)
   {
      if (FT_Load_Char(mFace, c, FT_LOAD_RENDER))
      {
         LOG_ERROR("Failed to load glyph: (%1)", c);
         continue;
      }

      Maybe<TextureAtlas::Region> region = mAtlas.Allocate(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows);
      if (!region) {
         LOG_ERROR("Failed to allocate region for glyph: (%1)", c);
         continue;
      }

      //LOG_INFO("Region allocated: (%1 %2) (%3 %4)", region->x, region->y, region->w, region->h);
      mAtlas.Fill(*region, mFace->glyph->bitmap.buffer, mFace->glyph->bitmap.width);

      characters.push_back({
         glm::ivec2(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows),
         glm::ivec2(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top),
         mFace->glyph->advance.x,
         *region
      });
   }

   return Success;
}
   
glm::vec2 Font::GetSizeOfRenderedText(const std::string& text)
{
   glm::vec2 result(0, 28);
   GLfloat cursor = 0;
   
   for (auto c : text)
   {
      if (c == '\n')
      {
         cursor = 0;
         result.y += 28;   // TODO-EF: Actual line-height
         continue;
      }
      
      Character ch = characters[c];
      
      cursor += ch.bearing.x;
      cursor += (ch.advance >> 6);
      result.x = fmax(cursor, result.x);  // Longest line sets the width
   }
   
   return result;
}

std::vector<Font::CharacterVertexUV> Font::Write(GLfloat x, GLfloat y, GLfloat availableWidth, GLfloat scale, const std::string& text, Alignment alignment)
{
   GLfloat initialCursor = x;
   GLfloat cursor = x;
   std::vector<CharacterVertexUV> result;

   for (auto c : text)
   {
      if (c == '\n')
      {
         cursor = x;
         y -= 28 * scale;   // TODO-EF: Actual line-height
         continue;
      }

      Character ch = characters[c];

      GLfloat xpos = cursor + ch.bearing.x * scale;
      GLfloat ypos = y + (ch.bearing.y - ch.size.y) * scale;
      GLfloat w = ch.size.x * scale;
      GLfloat h = ch.size.y * scale;

      GLfloat xuv = float(ch.region.x) / mAtlas.GetWidth();
      GLfloat yuv = float(ch.region.y) / mAtlas.GetHeight();
      GLfloat wuv = float(ch.region.w) / mAtlas.GetWidth();
      GLfloat huv = float(ch.region.h) / mAtlas.GetHeight();

      result.push_back({
         glm::vec2(xpos, ypos), 
         glm::vec2(xuv, yuv + huv)
      });
      result.push_back({
         glm::vec2(xpos + w, ypos + h),
         glm::vec2(xuv + wuv, yuv)
      });

      cursor += (ch.advance >> 6) * scale;
   }
   
   // Adjust for Right alignment or Center alignment
   GLfloat totalWidth = cursor - initialCursor;
   GLfloat nudgeRight = 0;
   
   if (alignment == Center) 
      nudgeRight = (availableWidth - totalWidth) / 2.0f;
   else if (alignment == Right)
      nudgeRight = availableWidth - totalWidth;
      
   for (auto& v : result)
   {
      v.position.x += nudgeRight;
   }

   return result;
}
   
Font::~Font()
{
   if (FT_Done_Face(mFace))
   {
      LOG_ERROR("Failed to clean up font face");
   }
}

FontManager::FontManager() : mValid(false)
{
   mFonts.clear();

   if (FT_Init_FreeType(&mLibrary))
   {
      LOG_ERROR("Could not init FreeType Library");
   }

   mValid = true;
}

FontManager::~FontManager() throw()
{
   mFonts.clear();
   FT_Done_FreeType(mLibrary);
}

Maybe<Font*> FontManager::GetFont(const std::string& path)
{
   assert(mValid);

   auto existing = mFonts.find(path);
   if (existing != mFonts.end())
   {
      return existing->second.get();
   }

   // Attempt to load the font.
   std::unique_ptr<Font> newFont = std::make_unique<Font>();
   Maybe<void> result = newFont->Load(mLibrary, path);
   if (result)
   {
      auto insertion = mFonts.emplace(path, std::move(newFont));
      return insertion.first->second.get();
   }

   return result.Failure();
}

}; // namespace Engine

}; // namespace Graphics

}; // namespace CubeWorld
