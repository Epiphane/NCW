// By Thomas Steinke

#include <cstdlib>
#include <GL/glew.h>
#include <algorithm>

#include <Engine/Core/Format.h>
#include <Engine/Logger/Logger.h>

#include "TextureAtlas.h"

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

   TextureAtlas::TextureAtlas(GLsizei width, GLsizei height, size_t depth)
      : mNodes{{1, 1, width - 2, height - 2}} // One pixel border around the whole atlas.
      , mWidth(width)
      , mHeight(height)
      , mDepth(depth)
      , mUsed(0)
      , mTexture(0)
      , mData(nullptr)
      , mDirty(true)
   {
      assert(depth == 1 || depth == 3 || depth == 4);

      mData = new uint8_t[width * height * depth];
   }

   TextureAtlas::TextureAtlas(TextureAtlas&& other)
      : mNodes{std::move(other.mNodes)}
      , mWidth(other.mWidth)
      , mHeight(other.mHeight)
      , mDepth(other.mDepth)
      , mUsed(other.mUsed)
      , mTexture(other.mTexture)
      , mData(other.mData)
      , mDirty(other.mDirty) 
   {
      other.mData = nullptr;
      other.mDirty = true;
   }

   TextureAtlas::~TextureAtlas()
   {
      if (mData != nullptr) 
      {
         free(mData);
      }
   }

   Maybe<TextureAtlas::Region> TextureAtlas::Allocate(GLsizei width, GLsizei height)
   {
      Region alloc;
      
      GLsizei bestY = -1;
      std::forward_list<Region>::iterator best = mNodes.end();
      for (auto node = mNodes.begin(); node != mNodes.end(); ++node)
      {
         // Don't consider a node if it's too far to the right.
         if (node->x + width > mWidth - 1)
         {
            // break instead of continue, since (see the definition of mNodes)
            // the list is sorted.
            break;
         }

         GLsizei y = node->y;
         int64_t widthLeft = width - node->w;
         auto next = node;
         while (widthLeft > 0 && ++next != mNodes.end())
         {
            y = std::max(y, next->y);
            if (y + height > mHeight - 1)
            {
               // No longer fits.
               break;
            }

            widthLeft -= next->w;
         }

         // We found a match. y now represents the value we need in order to fit.
         if (widthLeft <= 0)
         {
            if (bestY < 0 || y < bestY || (y == bestY && node->w < best->w))
            {
               bestY = y;
               best = node;
               alloc.x = node->x;
               alloc.y = y;
               alloc.w = width;
               alloc.h = height;
            }
         }
      }

      if (best == mNodes.end())
      {
         return Maybe<void>();
      }

      if (best->w <= width)
      {
         //LOG_INFO("Resize node: (%1 %2) (%3) -> (%4 %5) (%6)", best->x, best->y, best->w, best->x, alloc.y + height, width);
         
         // If we're larger than one node, resize it and any subsequent nodes.
         best->w = width;
         best->y = alloc.y + height;

         auto next = std::next(best);
         while (next != mNodes.end() && next->x + next->w < best->x + best->w)
         {
            //LOG_INFO("Erase node: (%1 %2) (%3)", next->x, next->y, next->w);
            mNodes.erase_after(best);
            next = std::next(best);
         }

         if (next != mNodes.end() && next->x < best->x + best->w)
         {
            GLsizei diff = (best->x + best->w) - next->x;
            //LOG_INFO("Resize node: (%1 %2) (%3) -> (%4 %5) (%6)", next->x, next->y, next->w, next->x + diff, next->y, next->w - diff);
            next->x += diff;
            next->w -= diff;
         }
      }
      else
      {
         // Resize the best node to reflect our added region,
         // and create a new one to reflect the extra space left over.
         Region newNode;
         newNode.x = best->x + width;
         newNode.w = best->w - width;
         newNode.y = best->y;

         best->y = alloc.y + height;
         best->w = width;

         //LOG_INFO("Nodes: (%1 %2) (%3) + (%4 %5) (%6)",
         //   best->x, best->y, best->w,
         //   newNode.x, newNode.y, newNode.w);

         mNodes.insert_after(best, newNode);
      }

      return alloc;
   }

   void TextureAtlas::Fill(Region region, const void* data, const size_t stride)
   {
      assert(region.x > 0);
      assert(region.y > 0);
      assert(region.x < mWidth - 1);
      assert(region.x + region.w <= mWidth - 1);
      assert(region.y < mHeight - 1);
      assert(region.y + region.h <= mHeight - 1);

      assert((data == nullptr && region.h == 0 && region.w == 0) ||
         (data != nullptr && region.h > 0 && region.w > 0));

      mDirty = false;

      for (int i = 0; i < region.h; ++i)
      {
         memcpy(mData + ((region.y + i) * mWidth + region.x) * sizeof(uint8_t) * mDepth,
            (uint8_t*)data + (i * stride) * sizeof(uint8_t), region.w * sizeof(uint8_t) * mDepth);
      }
   }

   GLuint TextureAtlas::GetTexture()
   {
      if (!mTexture)
      {
         glGenTextures(1, &mTexture);
         glBindTexture(GL_TEXTURE_2D, mTexture);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
         mDirty = true;
      }

      if (mDirty)
      {
         glTexImage2D(
            GL_TEXTURE_2D,    // target
            0,                // level
            GL_RED,           // internal format
            mWidth,           // width
            mHeight,          // height
            0,                // border: must be 0 (the fk)
            GL_RED,           // format
            GL_UNSIGNED_BYTE, // type
            mData             // data
         );

         mDirty = false;
      }

      return mTexture;
   }
   
}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
