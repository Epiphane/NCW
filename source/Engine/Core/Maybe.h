// By Thomas Steinke

#pragma once

#include <cassert>
#include <memory>

#include "Either.h"
#include "Failure.h"

namespace CubeWorld
{
/*
template<typename T> class Maybe;

template<> class Maybe<void>
{
public:
   Maybe() : isSuccess(true) {};
   Maybe(const Failure& failure) : failure(failure), isSuccess(false) {};
   Maybe(const Maybe<void>& other) : failure(other.failure), isSuccess(other.isSuccess) {};
   ~Maybe() {}

   bool Succeeded() { return isSuccess; }
   bool Failed() { return !Succeeded(); }

   Failure& Failure() { assert(Failed()); return failure; }

   constexpr Maybe& operator=(const Maybe& other)
   { 
      if (isSuccess = other.isSuccess)
      {
         failure = other.failure;
      }
      return *this;
   }

   constexpr void operator*() { assert(false); }
   constexpr void* operator->() { assert(false); }
   constexpr bool operator !() const { return !isSuccess; }

protected:
   CubeWorld::Failure failure;

   bool isSuccess;
};*/

template <typename T, typename FailureType>
class MaybeType : protected Either<
   typename std::conditional<std::is_void<T>::value, std::nullptr_t, T>::type,
   FailureType
>
{
public:
   using ValueType = typename std::conditional<std::is_void<T>::value, std::nullptr_t, T>::type;
   using Either = Either<
      typename std::conditional<std::is_void<T>::value, std::nullptr_t, T>::type,
      FailureType
   >;

   // Default constructor is not a success.
   constexpr MaybeType() : Either(FailureType{}) {};

   // Move and copy constructors for each side.
   constexpr MaybeType(const ValueType& value) : Either(value) {};
   constexpr MaybeType(ValueType&& value) : Either(std::move(value)) {};
   constexpr MaybeType(const FailureType& failure) : Either(failure) {};
   constexpr MaybeType(FailureType&& failure) : Either(std::move(failure)) {};

public:
   // Success and failure operators.
   bool Succeeded() const { return this->IsLeft(); }
   bool Failed() const { return this->IsRight(); }
   constexpr bool operator !() const { return Failed(); }
   constexpr operator bool() const { return Succeeded(); }

   // Retrieving the error.
   const FailureType& Failure() const { return this->Right(); }
   FailureType& Failure() { return this->Right(); }

   // Data access.
   const ValueType& Result() const { return this->Left(); }
   ValueType& Result() { return this->Left(); }

   constexpr ValueType& operator*() { assert(Succeeded()); return Result(); }
   constexpr T* operator->() { assert(Succeeded()); return &this->leftVal; }
};

template <typename T>
using Maybe = MaybeType<T, Failure>;

const std::nullptr_t Success = nullptr;

}; // namespace CubeWorld
