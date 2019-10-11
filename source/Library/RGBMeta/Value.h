// By Thomas Steinke

#pragma once

#include <detail/template_helpers.h>
#include "ValueHolder.h"

// namespace CubeWorld
// {

namespace meta
{

//
// Describes, at compile-time, a string mapping for Enum values
// to improve serialization.
//
template<typename EnumType>
class Value
{
public:
   Value(const std::string& name, const EnumType value) : name(name), value(value) {};

   const std::string& getName() const { return name; }
   EnumType getValue() const { return value; }

private:
   std::string name;
   EnumType value;
};

template <typename EnumType>
Value<EnumType> value(const std::string& name, EnumType value);

template <typename EnumType>
Value<EnumType> value(const std::string& name, EnumType value)
{
   return Value<EnumType>(name, value);
}

template <typename... Args>
auto values(Args&&... args)
{
   return std::make_tuple(std::forward<Args>(args)...);
}

template <typename EnumType>
const auto& getValues()
{
   return CubeWorld::Meta::ValueHolder<EnumType, decltype(registerValues<EnumType>())>::values;
}

// function used for registration of classes by user
template <typename EnumType>
inline auto registerValues();

template <typename EnumType>
inline auto registerValues()
{
   return std::make_tuple();
}

template <typename EnumType>
constexpr bool valuesRegistered()
{
   return !std::is_same<std::tuple<>, decltype(registerValues<EnumType>())>::value;
}

template <typename EnumType, typename F,
   typename = std::enable_if_t<valuesRegistered<EnumType>()>>
void doForAllValues(F&& f);

template <typename EnumType, typename F, typename>
void doForAllValues(F&& f)
{
   detail::for_tuple(std::forward<F>(f), getValues<EnumType>());
}

template <typename EnumType, typename = std::enable_if_t<valuesRegistered<EnumType>()>>
EnumType getValue(const std::string& name);

template <typename EnumType, typename>
EnumType getValue(const std::string& name)
{
   EnumType result;
   doForAllValues<EnumType>([&result, &name](const auto& item) {
      if (item.getName() == name)
      {
         result = item.getValue();
      }
   });
   return result;
}

template <typename EnumType, typename = std::enable_if_t<valuesRegistered<EnumType>()>>
std::string getName(const EnumType value);

template <typename EnumType, typename>
std::string getName(const EnumType value)
{
   std::string result;
   doForAllValues<EnumType>([&result, &value](const auto& item) {
      if (item.getValue() == value)
      {
         result = item.getName();
      }
   });
   return result;
}

}; // namespace meta

// }; // namespace CubeWorld
