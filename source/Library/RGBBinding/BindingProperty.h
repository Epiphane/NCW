// By Thomas Steinke

#pragma once

#include <iterator>
#include <string>
#include <tuple>
#include <vector>
#include <glm/glm.hpp>
#include <rapidjson/document.h>

#include <RGBDesignPatterns/Maybe.h>

#include "Array.h"

namespace CubeWorld
{

class BindingProperty
{
public:
   const static BindingProperty Null;
   static BindingProperty _;

public:
   // Type flags
   enum Type {
      kNullType = 0,
      kTrueType = 1,
      kFalseType = 2,
      kNumberType = 3,
      kStringType = 4,
      kObjectType = 5,
      kArrayType = 6,
   };

public:
   // Forward declarations
   struct KeyVal;

   // Iterator types
   template<typename Property> class IteratorType;
   template<typename Property> class PairIteratorType;
   typedef IteratorType<BindingProperty> Iterator;
   typedef IteratorType<const BindingProperty> ConstIterator;
   typedef PairIteratorType<BindingProperty> PairIterator;
   typedef PairIteratorType<const BindingProperty> ConstPairIterator;

   // More specific iterators
   template<typename Property, typename It> class ArrayType;
   template <typename Property> class ArrayIteratorType;
   template<typename Property, typename It> class ObjectType;
   template <typename Property, typename KeyVal> class ObjectIteratorType;

   typedef ArrayIteratorType<BindingProperty> ArrayIterator;
   typedef ArrayType<BindingProperty, ArrayIterator> Array;
   typedef ArrayIteratorType<const BindingProperty> ConstArrayIterator;
   typedef ArrayType<const BindingProperty, ConstArrayIterator> ConstArray;

   typedef ObjectIteratorType<BindingProperty, KeyVal> ObjectIterator;
   typedef ObjectType<BindingProperty, ObjectIterator> Object;
   typedef ObjectIteratorType<const BindingProperty, const KeyVal> ConstObjectIterator;
   typedef ObjectType<const BindingProperty, ConstObjectIterator> ConstObject;

public:
   BindingProperty() : flags(kNullFlag) { data.numVal.i64 = 0; };
   BindingProperty(const Type& type);
   BindingProperty(const BindingProperty& other);
   BindingProperty(BindingProperty&& other);
   ~BindingProperty();

   // Value initialization
   BindingProperty(bool value);
   BindingProperty(int32_t i);
   BindingProperty(int64_t i64);
   BindingProperty(uint32_t u);
   BindingProperty(uint64_t u64);
   BindingProperty(double d);
   BindingProperty(float f);
   BindingProperty(const char* s);
   BindingProperty(const std::string& s);
   BindingProperty(std::string&& s);
   BindingProperty(const glm::vec3& vec3);
   BindingProperty(const glm::vec4& vec4);

   // Assignment
   BindingProperty& operator=(const BindingProperty& other);
   BindingProperty& operator=(BindingProperty&& other);

   // Creates an element if the index does not exist.
   BindingProperty& operator[](const int& index);
   BindingProperty& operator[](const size_t& index);
   BindingProperty& operator[](const std::string& key);
   BindingProperty& operator[](const char* key);

   const BindingProperty& operator[](const int& index) const;
   const BindingProperty& operator[](const size_t& index) const;
   const BindingProperty& operator[](const std::string& key) const;
   const BindingProperty& operator[](const char* key) const;

   // Comparison
   bool operator==(const BindingProperty& other) const;
   inline bool operator!=(const BindingProperty& other) const { return !(*this == other); }

public:
   // Access and reading
   bool IsNull() const { return flags == uint16_t(kNullFlag); }
   bool IsBool() const { return (flags & kBoolFlag) != 0; }
   bool IsObject() const { return flags == uint16_t(kObjectFlag); }
   bool IsArray() const { return flags == uint16_t(kArrayFlag); }
   bool IsNumber() const { return (flags & kNumberFlag) != 0; }
   bool IsInt() const { return (flags & kIntFlag) != 0; }
   bool IsUint() const { return (flags & kUintFlag) != 0; }
   bool IsInt64() const { return (flags & kInt64Flag) != 0; }
   bool IsUint64() const { return (flags & kUint64Flag) != 0; }
   bool IsDouble() const { return (flags & kDoubleFlag) != 0; }
   bool IsString() const { return flags == uint16_t(kStringFlag); }
   bool IsVec3() const;
   bool IsVec4() const;

   const bool GetBooleanValue(const bool& defaultValue = false) const;
   const int32_t GetIntValue(const int32_t& defaultValue = 0) const;
   const uint32_t GetUintValue(const uint32_t& defaultValue = 0) const;
   const int64_t GetInt64Value(const int64_t& defaultValue = 0) const;
   const uint64_t GetUint64Value(const uint64_t& defaultValue = 0) const;
   const double GetDoubleValue(const double& defaultValue = 0) const;
   const float GetFloatValue(const float& defaultValue = 0) const;
   const std::string GetStringValue(const std::string& defaultValue = "") const;
   glm::vec3 GetVec3(const glm::vec3& defaultValue = {0, 0, 0}) const;
   glm::vec4 GetVec4(const glm::vec4& defaultValue = {0, 0, 0, 0}) const;

   inline operator std::string() const { return GetStringValue(); }

   Array AsArray();
   ConstArray AsArray() const;
   Object AsObject();
   ConstObject AsObject() const;

   bool Has(const std::string& key);
   ObjectIterator Find(const std::string& key);
   ConstObjectIterator Find(const std::string& key) const;

   Iterator begin();
   Iterator end();
   ConstIterator begin() const;
   ConstIterator end() const;
   PairIterator begin_pairs();
   PairIterator end_pairs();
   ConstPairIterator begin_pairs() const;
   ConstPairIterator end_pairs() const;

   template <typename Property, typename Iterator>
   struct PairMaker {
      Iterator begin() const { return obj.begin_pairs(); }
      Iterator end() const { return obj.end_pairs(); }
      Property& obj;
   };

   PairMaker<BindingProperty, PairIterator> pairs()
   {
      return PairMaker<BindingProperty, PairIterator>{*this};
   }

   PairMaker<const BindingProperty, ConstPairIterator> pairs() const
   {
      return PairMaker<const BindingProperty, ConstPairIterator>{*this};
   }

   /*
   constexpr bool operator !() const { return !this->operator bool(); }
   constexpr operator bool() const
   {
      if ((flags == uint16_t(kNullFlag)) != 0) { return false; }
      if ((flags & kBoolFlag) != 0) { return flags == uint16_t(kTrueFlag); }
      if ((flags & kIntFlag) != 0) { return data.numVal.i.i != 0; }
      if ((flags & kUintFlag) != 0) { return data.numVal.u.u != 0; }
      if ((flags & kInt64Flag) != 0) { return data.numVal.i64 != 0; }
      if ((flags & kUint64Flag) != 0) { return data.numVal.u64 != 0; }
      if ((flags & kDoubleFlag) != 0) { return data.numVal.d != 0; }
      if ((flags & kStringFlag) != 0) { return !data.stringVal.empty(); }
      if ((flags & kObjectFlag) != 0) { return data.objectVal.size() > 0; }
      if ((flags & kArrayFlag) != 0) { return data.arrayVal.size() > 0; }
      return false;
   }
   */

public:
   // Modification
   BindingProperty& SetNull()                { this->~BindingProperty(); new (this) BindingProperty();    return *this; }
   BindingProperty& SetBool(bool val)        { this->~BindingProperty(); new (this) BindingProperty(val); return *this; }
   BindingProperty& SetInt(int i)            { this->~BindingProperty(); new (this) BindingProperty(i);   return *this; }
   BindingProperty& SetUint(unsigned u)      { this->~BindingProperty(); new (this) BindingProperty(u);   return *this; }
   BindingProperty& SetInt64(int64_t i64)    { this->~BindingProperty(); new (this) BindingProperty(i64); return *this; }
   BindingProperty& SetUint64(uint64_t u64)  { this->~BindingProperty(); new (this) BindingProperty(u64); return *this; }
   BindingProperty& SetDouble(double d)      { this->~BindingProperty(); new (this) BindingProperty(d);   return *this; }
   BindingProperty& SetFloat(float f)        { this->~BindingProperty(); new (this) BindingProperty(f);   return *this; }

   BindingProperty& SetString(const std::string& s) { this->~BindingProperty(); new (this) BindingProperty(s);            return *this; }
   BindingProperty& SetString(std::string&& s)      { this->~BindingProperty(); new (this) BindingProperty(std::move(s)); return *this; }

   BindingProperty& SetArray()  { this->~BindingProperty(); new (this) BindingProperty(kArrayType);  return *this; }
   BindingProperty& PushBack(BindingProperty val);
   inline BindingProperty& push_back(BindingProperty val) { return PushBack(val); }
   void PopBack();
   inline void pop_back() { return PopBack(); }

   BindingProperty& SetObject() { this->~BindingProperty(); new (this) BindingProperty(kObjectType); return *this; }
   BindingProperty& Set(const std::string& key, const BindingProperty& value);
   BindingProperty& Set(const std::string& key, BindingProperty&& value);

public:
   // This uses the rapidjson Handler pattern.
   template <typename Handler>
   Maybe<void> Write(Handler& handler) const;
   
public:
   // Iterators and such things
   template<typename Property>
   class IteratorType : public std::iterator<std::input_iterator_tag, Property> {
   public:
      IteratorType& operator++()
      {
         switch (mObject->flags)
         {
         case kArrayFlag:
            if (mIndex < mObject->data.arrayVal.size()) { ++mIndex; }
            break;
         case kObjectFlag:
            if (mIndex < mObject->data.objectVal.size()) { ++mIndex; }
            break;
         default:
            assert(false && "Dereferencing iterator on a property that is not an object or an array");
         }
         return *this;
      }

      bool operator==(const IteratorType& rhs) const
      {
         return mObject == rhs.mObject && mIndex == rhs.mIndex;
      }

      bool operator!=(const IteratorType& rhs) const
      {
         return !(*this == rhs);
      }

      Property& operator*() const
      {
         switch (mObject->flags)
         {
         case kArrayFlag:
            return mObject->data.arrayVal[mIndex];
         case kObjectFlag:
            return mObject->data.objectVal[mIndex].value;
         default:
            assert(false && "Dereferencing iterator on a property that is not an object or an array");
         }
         // This will never happen but it silences compiler warnings
         return *mObject;
      }

      Property* operator->() const
      {
         return &(this->operator*());
      }

      IteratorType& operator=(const IteratorType& other)
      {
         mObject = other.mObject;
         mIndex = other.mIndex;
         return *this;
      }

   private:
      friend class BindingProperty;
      IteratorType(Property* object, uint32_t index)
         : mObject(object)
         , mIndex(index)
      {};

   private:
      Property* mObject;
      uint32_t mIndex;
   };

   template<typename Property>
   class PairIteratorType : public std::iterator<std::input_iterator_tag, std::pair<BindingProperty, Property&>> {
   public:
      PairIteratorType& operator++() // prefix
      {
         switch (mObject->flags)
         {
         case kArrayFlag:
            if (mIndex < mObject->data.arrayVal.size()) { ++mIndex; }
            break;
         case kObjectFlag:
            if (mIndex < mObject->data.objectVal.size()) { ++mIndex; }
            break;
         default:
            assert(false && "Dereferencing iterator on a property that is not an object or an array");
         }
         return *this;
      }

      bool operator==(const PairIteratorType& rhs) const
      {
         return mObject == rhs.mObject && mIndex == rhs.mIndex;
      }

      bool operator!=(const PairIteratorType& rhs) const
      {
         return !(*this == rhs);
      }

      std::pair<BindingProperty, Property&> operator*() const
      {
         if (mObject->IsArray())
         {
            return {
               BindingProperty(mIndex),
               mObject->data.arrayVal[mIndex]
            };
         }
         else if (mObject->IsObject())
         {
            return {
               BindingProperty(mObject->data.objectVal[mIndex].key),
               mObject->data.objectVal[mIndex].value
            };
         }
         assert(false && "Dereferencing iterator on a property that is not an object or an array");
         return std::pair<BindingProperty, Property&>{BindingProperty{}, _};
      }

      PairIteratorType& operator=(const PairIteratorType& other)
      {
         mObject = other.mObject;
         mIndex = other.mIndex;
         return *this;
      }

   private:
      friend class BindingProperty;

      PairIteratorType(Property* object, uint32_t index)
         : mObject(object)
         , mIndex(index)
      {}

   private:
      Property* mObject;
      uint32_t mIndex;
   };

   template <typename Property>
   class ArrayIteratorType : public std::iterator<std::input_iterator_tag, BindingProperty> {
   public:
      ArrayIteratorType& operator++() // prefix
      {
         if (mIndex < mObject->data.arrayVal.size()) { ++mIndex; }
         return *this;
      }

      bool operator==(const ArrayIteratorType& rhs) const
      {
         return mObject == rhs.mObject && mIndex == rhs.mIndex;
      }

      bool operator!=(const ArrayIteratorType& rhs) const
      {
         return !(*this == rhs);
      }

      Property& operator*() const
      {
         return mObject->data.arrayVal[mIndex];
      }

      Property* operator->() const
      {
         return &mObject->data.arrayVal[mIndex];
      }

   private:
      friend class BindingProperty;

      ArrayIteratorType(Property* object, uint32_t index)
         : mObject(object)
         , mIndex(index)
      {};

   public:
      ArrayIteratorType & operator=(const ArrayIteratorType& other)
      {
         mObject = other.mObject;
         mIndex = other.mIndex;
         return *this;
      }

   private:
      Property * mObject;
      uint32_t mIndex;
   };

   template<typename Property, typename It>
   class ArrayType {
   public:
      ArrayType(Property* object) : mObject(object) {};

      It begin() { return It(mObject, 0); }
      It end() { return It(mObject, mObject->data.arrayVal.size()); }

   private:
      Property* mObject;
   };

   template <typename Property, typename KV>
   class ObjectIteratorType : public std::iterator<std::input_iterator_tag, KV> {
   public:
      ObjectIteratorType& operator++() // prefix
      {
         if (mIndex < mObject->data.objectVal.size()) { ++mIndex; }
         return *this;
      }

      bool operator==(const ObjectIteratorType& rhs) const
      {
         return mObject == rhs.mObject && mIndex == rhs.mIndex;
      }

      bool operator!=(const ObjectIteratorType& rhs) const
      {
         return !(*this == rhs);
      }

      // https://stackoverflow.com/questions/53721714/why-does-structured-binding-not-work-as-expected-on-struct
      // Workaround: don�t use const on the struct.
      // In order to keep this const (so it doesn't change the actual properties), just do it the hard way.
      KV& operator*() const
      {
         return mObject->data.objectVal[mIndex];
      }

      KV* operator->() const
      {
         return &(this->operator*());
      }

   private:
      friend class BindingProperty;

      ObjectIteratorType(Property* object, uint32_t index)
         : mObject(object)
         , mIndex(index)
      {};

   public:
      ObjectIteratorType& operator=(const ObjectIteratorType& other)
      {
         mObject = other.mObject;
         mIndex = other.mIndex;
         return *this;
      }

   private:
      Property* mObject;
      uint32_t mIndex;
   };

   template<typename Property, typename It>
   class ObjectType {
   public:
      ObjectType(Property* object) : mObject(object) {};

      It begin() { return It(mObject, 0); }
      It end() { return It(mObject, mObject->data.objectVal.size()); }

   private:
      Property* mObject;
   };

private:
   // Guess where this came from
   // Size: 8 bytes
   union Number {
#if RAPIDJSON_ENDIAN == RAPIDJSON_LITTLEENDIAN
      struct I {
         int i;
         char padding[4];
      }i;
      struct U {
         unsigned u;
         char padding2[4];
      }u;
#else
      struct I {
         char padding[4];
         int i;
      }i;
      struct U {
         char padding2[4];
         unsigned u;
      }u;
#endif
      int64_t i64;
      uint64_t u64;
      double d;
   };  

   union Data {
      Data() { numVal.i64 = 0; }
      ~Data() { /* Handled manually */ }

      Number numVal;
      std::string stringVal;
      RGBBinding::Array<BindingProperty> arrayVal;
      RGBBinding::Array<KeyVal> objectVal;
   };

private:
   enum Flags {
      kBoolFlag = 0x0008,
      kNumberFlag = 0x0010,
      kIntFlag = 0x0020,
      kUintFlag = 0x0040,
      kInt64Flag = 0x0080,
      kUint64Flag = 0x0100,
      kDoubleFlag = 0x0200,

      // Initial flags of different types.
      kNullFlag = kNullType,
      kTrueFlag = kTrueType | kBoolFlag,
      kFalseFlag = kFalseType | kBoolFlag,
      kNumberIntFlag = kNumberType | kNumberFlag | kIntFlag | kInt64Flag,
      kNumberUintFlag = kNumberType | kNumberFlag | kUintFlag | kUint64Flag | kInt64Flag,
      kNumberInt64Flag = kNumberType | kNumberFlag | kInt64Flag,
      kNumberUint64Flag = kNumberType | kNumberFlag | kUint64Flag,
      kNumberDoubleFlag = kNumberType | kNumberFlag | kDoubleFlag,
      kNumberAnyFlag = kNumberType | kNumberFlag | kIntFlag | kInt64Flag | kUintFlag | kUint64Flag | kDoubleFlag,
      kStringFlag = kStringType,
      kObjectFlag = kObjectType,
      kArrayFlag = kArrayType,

      kTypeMask = 0x07
   };

private:
   // TODO with some clever union magic this type variable can actually be
   // included _IN_ the data. See rapidjson's documentation, it's really neat.
   // Problem is, it requires being intentional about the standard structures,
   // since std::string and std::vector don't make any guarantees about their
   // size. Instead of trying to be clever or asserting on their size, I
   // opted to just widen this object by 2 bytes. Boo hoo.
   Data data;
   uint16_t flags = kNullFlag;
};

struct BindingProperty::KeyVal {
   inline bool operator==(const KeyVal& other) const
   {
      return key == other.key && value == other.value;
   }

   inline bool operator!=(const KeyVal& other) const { return !(*this == other); }

   std::string key;
   BindingProperty value;
};

// BinUtility
// For comparisons, both values must be convertible to a BindingProperty
template<typename T,
typename std::enable_if<!std::is_same<T, BindingProperty>::value, int>::type = 0>
inline bool operator==(const BindingProperty& prop, const T& other)
{
   return prop == BindingProperty(other);
}

template<typename T,
typename std::enable_if<!std::is_same<T, BindingProperty>::value, int>::type = 0>
inline bool operator==(const T& other, const BindingProperty& prop)
{
   return BindingProperty(other) == prop;
}

template<typename T, typename std::enable_if<!std::is_same<T, BindingProperty>::value, int>::type = 0>
inline bool operator!=(const BindingProperty& prop, const T& other)
{
   return prop != BindingProperty(other);
}

template<typename T,
typename std::enable_if<!std::is_same<T, BindingProperty>::value, int>::type = 0>
inline bool operator!=(const T& other, const BindingProperty& prop) { return !(other == prop); }

template<typename Handler>
Maybe<void> BindingProperty::Write(Handler& handler) const
{
   ConstArrayIterator it(this, 0), end(this, 0);

#define HANDLE_ERROR(op, error) if (!op) { return Failure{error}; }
   switch (flags & kTypeMask)
   {
   case kNullType:
      HANDLE_ERROR(handler.Null(), "Failed to write null value");
      break;
   case kTrueType:
   case kFalseType:
      HANDLE_ERROR(handler.Bool(GetBooleanValue()), "Failed to write boolean value");
      break;
   case kNumberType:
      if (IsDouble()) { HANDLE_ERROR(handler.Double(data.numVal.d), "Failed to write double value"); }
      else if (IsInt()) { HANDLE_ERROR(handler.Int(data.numVal.i.i), "Failed to write int value"); }
      else if (IsUint()) { HANDLE_ERROR(handler.Uint(data.numVal.u.u), "Failed to write unsigned int value"); }
      else if (IsInt64()) { HANDLE_ERROR(handler.Int64(data.numVal.i64), "Failed to write 64-bit int value"); }
      else if (IsUint64()) { HANDLE_ERROR(handler.Uint64(data.numVal.u64), "Failed to write unsigned 64-bit int value"); }
      else { return Failure{"Unhandled number type: %1", flags}; }
      break;
   case kStringType:
      HANDLE_ERROR(handler.String(data.stringVal.c_str(), data.stringVal.size(), false), "Failed to write string value");
      break;
   case kObjectType:
      HANDLE_ERROR(handler.StartObject(), "Failed to start object");
      for (const KeyVal& kv : data.objectVal)
      {
         if (!handler.Key(kv.key.c_str(), kv.key.size(), false))
         {
            return Failure{"Failed to write key %1", kv.key};
         }
         if (Maybe<void> result = kv.value.Write(handler); !result)
         {
            return result.Failure().WithContext("Failed to write value for %1", kv.key);
         }
      }
      HANDLE_ERROR(handler.EndObject(data.objectVal.size()), "Failed to end object");
      break;
   case kArrayType:
      HANDLE_ERROR(handler.StartArray(), "Failed to start array");
      end = AsArray().end();
      for (it = AsArray().begin(); it != end; ++it)
      {
         if (Maybe<void> result = it->Write(handler); !result)
         {
            return result.Failure().WithContext("Failed to write index %1", it.mIndex);
         }
      }
      HANDLE_ERROR(handler.EndArray(data.arrayVal.size()), "Failed to end array");
      break;
   default:
      return Failure{"Unhandled type flag: %1", flags & kTypeMask};
   }

   return Success;
}

}; // namespace CubeWorld
