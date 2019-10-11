// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

namespace CubeWorld
{

namespace Engine
{

/**
 * Pool is a more flexible way for managing a list of objects.
 *
 * It guarantees cache-friendliness by managing the memory in large blocks.
 * Pointers are only invalidated when the pool is destroyed.
 *
 * Pool is a templated method for creating a BasePool. This base class exists
 * so that the entity manager can maintain a list of BasePools without providing
 * a type at construction time.
 */
class BasePool {
public:
   // explicit keeps from unintentionally using a move or copy constructor.
   // Create a new object pool for elements size {elementSize}, allocating
   // enough space for {blockSize} elements at a time.
   explicit BasePool(size_t elementSize, size_t blockSize = 2048)
      : mElementSize(elementSize)
      , mBlockSize(blockSize)
      , mSize(0)
      , mCapacity(0)
   {};
   virtual ~BasePool() {};

   size_t size() const { return mSize; }
   size_t capacity() const { return mCapacity; }
   size_t blocks() const { return mBlocks.size(); }

   // Guarantee that there is space for n elements.
   inline void expand(size_t n)
   {
      if (n > mSize) {
         if (n > mCapacity) {
            reserve(n);
         }
         mSize = n;
      }
   }

   inline void reserve(size_t n)
   {
      while (n > mCapacity)
      {
         std::unique_ptr<char> block{new char[mElementSize * mBlockSize]};
         mBlocks.push_back(std::move(block));
         mCapacity += mBlockSize;
      }
   }

   inline void* get(size_t n)
   {
      assert(n < mSize);
      return mBlocks[n / mBlockSize].get() + (n % mBlockSize) * mElementSize;
   }

   inline const void* get(size_t n) const
   {
      assert(n < mSize);
      return mBlocks[n / mBlockSize].get() + (n % mBlockSize) * mElementSize;
   }

   // The type-specific implementation handles destruction of objects.
   virtual void destroy(size_t n) = 0;

protected:
   size_t mElementSize;

   std::vector<std::unique_ptr<char>> mBlocks;
   // Block size in elements, not bytes.
   size_t mBlockSize;
   size_t mSize;
   size_t mCapacity;
};

template<typename T, size_t ChunkSize = 2048>
class Pool : public BasePool {
public:
   Pool() : BasePool(sizeof(T), ChunkSize) {};

   virtual void destroy(size_t n) override
   {
      assert(n < mSize);
      T *ptr = static_cast<T*>(get(n));
      ptr->~T();
   }
};

}; // namespace Engine

}; // namespace CubeWorld
