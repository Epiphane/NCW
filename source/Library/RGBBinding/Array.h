// By Thomas Steinke

#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>

#include <rapidjson/allocators.h>

namespace CubeWorld
{

class BindingProperty;

namespace RGBBinding
{

// Size: 12 or 16 bytes (32-bit or 64-bit pointer)
template<typename Data>
class Array {
public:
   static constexpr size_t INITIAL_SIZE = 4;

public:
   Array() : mSize(0), mCapacity(0), mData(nullptr)
   {
      reserve(INITIAL_SIZE);
   };

   Array(const Array& other) : mSize(0), mCapacity(0), mData(nullptr)
   {
      reserve(other.mSize);
      for (size_t i = 0; i < other.mSize; ++i)
      {
         new (&mData[i]) Data(other.mData[i]);
      }
      mSize = other.mSize;
   }

   Array(Array&& other) noexcept
   {
      mCapacity = other.mCapacity;
      mSize = other.mSize;
      mData = other.mData;
      other.mData = nullptr;
      other.mSize = other.mCapacity = 0;
   }

   ~Array()
   {
      clear();

      if (mData)
      {
         std::free(mData);
         mData = nullptr;
      }
      mCapacity = 0;
   }

   Array& operator=(const Array& other)
   {
      clear();

      reserve(other.mSize);
      for (size_t i = 0; i < other.mSize; ++i)
      {
         mData[i] = other.mData[i];
      }
      mSize = other.mSize;
      return *this;
   }

   Array& operator=(Array&& other)
   {
      clear();

      if (mData)
      {
         std::free(mData);
      }

      mCapacity = other.mCapacity;
      mSize = other.mSize;
      mData = other.mData;
      other.mData = nullptr;
      other.mSize = other.mCapacity = 0;
      return *this;
   }

   bool operator==(const Array& other) const
   {
      if (mSize != other.mSize) { return false; }
      for (size_t i = 0; i < mSize; ++i)
      {
         if (mData[i] != other.mData[i])
         {
            return false;
         }
      }
      return true;
   }

   inline bool operator!=(const Array& other) const
   {
      return !(*this == other);
   }

   void clear()
   {
      while (mSize > 0)
      {
         mData[--mSize].~Data();
      }
   }

   size_t size() const { return mSize; }
   size_t capacity() const { return mCapacity; }

   Data& operator[](size_t index)
   {
      assert(index < mSize && "Index array out of bounds");
      return mData[index];
   }

   const Data& operator[](size_t index) const
   {
      assert(index < mSize && "Index array out of bounds");
      return mData[index];
   }

   void push_back(const Data& data)
   {
      push_back(BindingProperty(data));
   }

   void push_back(Data&& data)
   {
      if (mSize >= mCapacity)
      {
         if (mCapacity * 2 < INITIAL_SIZE)
         {
            reserve(INITIAL_SIZE);
         }
         else
         {
            reserve(mCapacity * 2);
         }
         // idk what to do if it doesn't work; I guess we'll just pray.
      }
      memset(&mData[mSize], 0, sizeof(Data));
      mData[mSize] = std::move(data);
      mSize++;
   }

   void pop_back()
   {
      assert(mSize > 0 && "Attempting to pop_back on an empty array");
      mData[--mSize].~Data();
   }

   void resize(size_t size)
   {
      if (mSize > size)
      {
         for (size_t i = mSize - 1; i >= size; --i)
         {
            mData[i].~Data();
         }
         mSize = size;
         return;
      }

      if (mCapacity < size)
      {
         reserve(std::max(mCapacity * 2, size));
      }

      while (mSize < size)
      {
         new (&mData[mSize++]) Data();
      }
   }

   void reserve(size_t size)
   {
      if (mCapacity >= size)
      {
         return;
      }

      size_t newCapacity = size;
      void* newData = std::malloc(newCapacity * sizeof(Data));
      if (newData != nullptr)
      {
         std::memcpy(newData, mData, mCapacity * sizeof(Data));
         if (mData != nullptr) { std::free(mData); }
         mData = static_cast<Data*>(newData);

         mCapacity = newCapacity;
      }
   }

public:
   // Iterators and such
   template <typename Arr, typename D>
   class iterator_type {
   public:
      // Optimization potential: use a simple Data pointer
      // Unsafe? Sure. Clever? No doubt.
      iterator_type& operator++()
      {
         ++mIndex;
         return *this;
      }

      bool operator==(const iterator_type& rhs) const
      {
         return mArray == rhs.mArray && mIndex == rhs.mIndex;
      }

      bool operator!=(const iterator_type& rhs) const
      {
         return !(*this == rhs);
      }

      D& operator*() const
      {
         assert(mIndex < mArray->mSize && "Array index out of bounds");
         return mArray->mData[mIndex];
      }

      iterator_type& operator=(const iterator_type& other)
      {
         mArray = other.mArray;
         mIndex = other.mIndex;
         return *this;
      }

   private:
      friend class Array;

      iterator_type(Arr* array, size_t index)
         : mArray(array)
         , mIndex(index)
      {}

   private:
      Arr* mArray;
      size_t mIndex;
   };

   typedef iterator_type<Array, Data> iterator;
   typedef iterator_type<const Array, const Data> const_iterator;

   iterator begin() { return iterator(this, 0); }
   iterator end() { return iterator(this, mSize); }
   const_iterator begin() const { return const_iterator(this, 0); }
   const_iterator end() const { return const_iterator(this, mSize); }

private:
   size_t mSize;
   size_t mCapacity;
   Data* mData;
};

}; // namespace RGBBinding

}; // namespace CubeWorld
