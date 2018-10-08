// By Thomas Steinke

#pragma once

#include <algorithm>
#include <cstring>
#include <vector>

#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>

namespace CubeWorld
{

namespace Engine
{

//
// Base aggregatpr class, only used for insertion into collections.
//
// Family is used for registration by the UIRoot.
//
struct BaseUIAggregator {
public:
   virtual void Update() = 0;
   virtual void Render() = 0;

public:
   typedef size_t Family;

   // This gets incremented with each unique call to UIAggregator<D>::GetFamily();
   static Family sNumFamilies;
};

//
// UIAggregator serves as a single-point renderer for a group of
// similar UIElements. For example, a hundred images simply consist
// of two glm::vec2 pairs: one for position, and one for UVs. In
// this case, 2DTextureAggregator is used to collect all these images
// in a single VBO and render them in one batch, to speed up rendering.
//
// To implement a new type of rendering, simply extend UIAggregator<DataType>.
//
template <typename DataType>
class UIAggregator : public BaseUIAggregator {
public:
   //
   // A Region represents a block of space in the VBO that was claimed.
   //
   class Region {
   public:
      Region() : aggregator(nullptr), begin(0), elements(0) {};
      Region(UIAggregator* aggregator, size_t begin, size_t elements)
         : aggregator(aggregator)
         , begin(begin)
         , elements(elements)
      {};

      // Set the data in this region. data must contain size() DataTypes.
      void Set(DataType* data) { aggregator->Set(*this, data); }

      DataType* data() { return &aggregator->mData[begin]; }
      size_t index() const { return begin; }
      size_t size() const { return elements; }

   protected:
      friend class UIAggregator;
      UIAggregator* aggregator;
      size_t begin;
      size_t elements;
   };

public:
   UIAggregator()
      : mVBO(Graphics::VBO::Vertices)
      , mData{}
      , mDirty(false)
      , mFree{}
   {};

   // Used internally for registration.
   // Defined here, because this is part of the template declaration,
   // so the compiler will consider it a different function for each component type.
   // That way each UIAggregator class gets a different family.
   static Family GetFamily()
   {
      static Family family = sNumFamilies++;
      assert(family < MAX_COMPONENTS);
      return family;
   }

   //
   // Reserve space for numElements elements in the VBO.
   // Return value is a region pointing to the data space.
   //
   virtual Region Reserve(const size_t& numElements)
   {
      for (Region& free : mFree)
      {
         // TODO do some cool "best fit" stuff here, to prevent fragmentation.
         if (free.elements >= numElements)
         {
            // Reserve the first {numElements} of this section.
            Region newRegion(this, free.begin, numElements);
            free.begin += numElements;
            free.elements -= numElements;
            return newRegion;
         }
      }

      // No regions left! Make a new one.
      Region result(this, mData.size(), numElements);
      mData.resize(mData.size() + numElements);
      return result;
   }

   //
   // Free up a region for use somewhere else.
   //
   void Free(const Region& region)
   {
      assert(false && "Unimplemented");
   }

   //
   // Set the data within a region.
   //
   void Set(const Region& region, DataType* data)
   {
      memcpy(&mData[region.begin], data, sizeof(DataType) * region.elements);
      mDirty = true;
   }

   //
   // Update the VBO if the data we're using has changed.
   //
   void Update() override
   {
      // TODO make use of glBufferSubData.
      if (mDirty)
      {
         mVBO.BufferData(sizeof(DataType) * mData.size(), mData.data(), GL_STATIC_DRAW);
         mDirty = false;
      }
   }

protected:
   Graphics::VBO mVBO;
   std::vector<DataType> mData;
   bool mDirty;

   // Regions that have been freed, for reuse.
   std::vector<Region> mFree;
};

   
}; // namespace Engine

}; // namespace CubeWorld
