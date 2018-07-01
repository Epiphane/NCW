// By Thomas Steinke

#pragma once

#include <memory>

namespace CubeWorld
{

   template<typename T> class Maybe;

   // Encapsulates a "success-or-error" case, instead of using the inconsistent
   // and hard to understand boolean.
   template<> class Maybe<void> {};

   template<typename T>
   class Maybe
   {
   public:
      constexpr Maybe() : hasValue(false) {};
      constexpr Maybe(const T& value) : value(value), hasValue(true) {};
      constexpr Maybe(T&& value) : value(std::move(value)), hasValue(true) {};
      constexpr Maybe(Maybe<void>) : hasValue(false) {};
      constexpr Maybe(const Maybe<T>& other) : hasValue(other.hasValue)
      {
         if (other.hasValue)
         {
            new (&value)T(other.value);
         }
      }

      ~Maybe()
      {
         if (hasValue)
         {
            value.~T();
         }
      }

      constexpr T get()
      {
         assert(hasValue);
         return value;
      }

      constexpr T operator*() { return get(); }
      constexpr T* operator->() { return &value; }
      constexpr operator bool() const { return hasValue; }

      constexpr Maybe& operator=(const T& val) { value = val; hasValue = true; return *this; }

   private:
      union {
         T value;
      };

      bool hasValue = false;
   };

}; // namespace CubeWorld
