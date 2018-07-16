// By Thomas Steinke

#pragma once

#include <memory>

namespace CubeWorld
{

template<typename L, typename R>
class Either
{
public:
   constexpr Either(const L& left) : leftVal(left), isLeft(true) {};
   constexpr Either(L&& left) : leftVal(std::move(left)), isLeft(true) {};
   constexpr Either(const R& right) : rightVal(right), isLeft(false) {};
   constexpr Either(R&& right) : rightVal(std::move(right)), isLeft(false) {};
   constexpr Either(const Either<L, R>& other) : isLeft(other.isLeft)
   {
      if (isLeft)
      {
         leftVal = other.leftVal;
      }
      else
      {
         rightVal = other.rightVal;
      }
   };
   ~Either()
   {
      if (isLeft)
      {
         leftVal.~L();
      }
      else
      {
         rightVal.~R();
      }
   }

   constexpr bool IsLeft() const { return isLeft; }
   constexpr bool IsRight() const { return !isLeft; }

   L& Left() { return leftVal; }
   //const L& Left() const { return leftVal; }
   R& Right() { return rightVal; }
   //const R& Right() const { return rightVal; }

   constexpr Either& operator=(const L& left) { leftVal = left; isLeft = true; return *this; }
   constexpr Either& operator=(const R& right) { rightVal = right; isLeft = false; return *this; }
   constexpr Either& operator=(const Either& other)
   {
      if (isLeft)
      {
         leftVal.~L();
      }
      else
      {
         rightVal.~R();
      }

      isLeft = other.isLeft;
      if (isLeft)
      {
         leftVal = other.leftVal;
      }
      else
      {
         rightVal = other.rightVal;
      }
      return *this;
   }

protected:
   union {
      L leftVal;
      R rightVal;
   };

   bool isLeft;
};

template<typename T>
class Pointer
{
public:
   constexpr Pointer(T* left) : leftVal(left), side(Left) {};
   constexpr Pointer(const std::unique_ptr<T> right) : rightVal(std::move(right)), side(Right) {};
   constexpr Pointer() : leftVal(nullptr), side(Empty) {};

   Pointer(Pointer&& other) : leftVal(nullptr)
   {
      if (other.side == Left)
      {
         this->leftVal = other.leftVal;
         other.leftVal = nullptr;
      }
      else if (other.side == Right)
      {
         this->rightVal = std::move(other.rightVal);
      }
      this->side = other.side;
      other.side = Empty;
   }

   ~Pointer()
   {
      
   };

   constexpr T* get()
   {
      if (side == Empty)
      {
         return nullptr;
      }
      else if (side == Left)
      {
         return leftVal;
      }
      return rightVal.get();
   }

   constexpr Pointer& operator=(T* left)
   {
      leftVal = left;
      side = Left;
      return *this;
   }
   constexpr Pointer& operator=(std::unique_ptr<T> right)
   {
      rightVal = std::move(right);
      this->side = Right;
      return *this;
   }
   constexpr Pointer& operator=(Pointer<T> other)
   {
      if (other.side == Left)
      {
         this->leftVal = other.leftVal;
         other.leftVal = nullptr;
      }
      else
      {
         this->rightVal = std::move(other.rightVal);
      }
      this->side = other.side;
      other.side = Empty;
      return *this;
   }

   constexpr operator bool() const { return get() != nullptr; }
   constexpr bool operator==(const Pointer& other) { return get() == other.get(); }
   constexpr bool operator==(const T* other) { return get() == other; }
   constexpr bool operator!=(const T* other) { return get() != other; }
   constexpr T* operator->() { return get(); }

private:
   union {
      T* leftVal;
      std::unique_ptr<T> rightVal;
   };

   enum { Left, Right, Empty } side;
};

}; // namespace CubeWorld
