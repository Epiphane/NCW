// By Thomas Steinke

#pragma once

#include <unordered_map>
#include <map>
#include <vector>

#include "BindingProperty.h"

namespace CubeWorld
{

// ------------------------------------------------------------------
// |                                                                |
// |                     BindingProperty Impl                       |
// |                                                                |
// ------------------------------------------------------------------

struct BindingProperty::KeyVal {
   inline bool operator==(const KeyVal& other) const
   {
      return key == other.key && value == other.value;
   }

   inline bool operator!=(const KeyVal& other) const { return !(*this == other); }

   std::string key;
   BindingProperty value;
};

template <typename T, typename>
BindingProperty::BindingProperty(T&& val) : BindingProperty(Binding::serialize(val)) {}

template <typename T, typename>
BindingProperty::BindingProperty(const T& val) : BindingProperty(Binding::serialize(val)) {}

template<typename T, typename>
T BindingProperty::Get() const
{
   T result;
   Binding::deserialize(result, *this);
   return result;
}

// BinUtility
// For comparisons, both values must be convertible to a BindingProperty
/*template<typename T,
typename std::enable_if<!std::is_same<T, BindingProperty>::value, int>::type = 0>
inline bool operator==(const BindingProperty& prop, const T& other)
{
   return prop == BindingProperty(other);
}
*/

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
   Type type = Type(flags & kTypeMask);
   switch (type)
   {
   case Type::Null:
      HANDLE_ERROR(handler.Null(), "Failed to write null value");
      break;
   case Type::True:
   case Type::False:
      HANDLE_ERROR(handler.Bool(GetBooleanValue()), "Failed to write boolean value");
      break;
   case Type::Number:
      if (IsDouble()) { HANDLE_ERROR(handler.Double(data.numVal.d), "Failed to write double value"); }
      else if (IsInt()) { HANDLE_ERROR(handler.Int(data.numVal.i.i), "Failed to write int value"); }
      else if (IsUint()) { HANDLE_ERROR(handler.Uint(data.numVal.u.u), "Failed to write unsigned int value"); }
      else if (IsInt64()) { HANDLE_ERROR(handler.Int64(data.numVal.i64), "Failed to write 64-bit int value"); }
      else if (IsUint64()) { HANDLE_ERROR(handler.Uint64(data.numVal.u64), "Failed to write unsigned 64-bit int value"); }
      else { return Failure{"Unhandled number type: {flags}", flags}; }
      break;
   case Type::String:
      HANDLE_ERROR(handler.String(data.stringVal.c_str(), (rapidjson::SizeType)data.stringVal.size(), false), "Failed to write string value");
      break;
   case Type::Object:
      HANDLE_ERROR(handler.StartObject(), "Failed to start object");
      for (const KeyVal& kv : data.objectVal)
      {
         if (!handler.Key(kv.key.c_str(), (rapidjson::SizeType)kv.key.size(), false))
         {
            return Failure{"Failed to write key {key}", kv.key};
         }
         if (Maybe<void> result = kv.value.Write(handler); !result)
         {
            return result.Failure().WithContext("Failed to write value for {key}", kv.key);
         }
      }
      HANDLE_ERROR(handler.EndObject((rapidjson::SizeType)data.objectVal.size()), "Failed to end object");
      break;
   case Type::Array:
      HANDLE_ERROR(handler.StartArray(), "Failed to start array");
      end = AsArray().end();
      for (it = AsArray().begin(); it != end; ++it)
      {
         if (Maybe<void> result = it->Write(handler); !result)
         {
            return result.Failure().WithContext("Failed to write index {index}", it.mIndex);
         }
      }
      HANDLE_ERROR(handler.EndArray((rapidjson::SizeType)data.arrayVal.size()), "Failed to end array");
      break;
   default:
      return Failure{"Unhandled type flag: {type}", flags & kTypeMask};
   }

   return Success;
}

}; // namespace CubeWorld

#include "BindingPropertyMeta.inl"
