#pragma once

#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <Meta.h>
#include <RGBMeta/Value.h>

// Commented to avoid circular dependency
// #include "BindingProperty.h"

namespace CubeWorld
{

class BindingProperty;

namespace Binding
{

// ------------------------------------------------------------------
// |                                                                |
// |                         Serialization                          |
// |                                                                |
// ------------------------------------------------------------------

template <typename Class, typename = std::enable_if_t<meta::isRegistered<Class>()>>
BindingProperty serialize(const Class & obj);

template <typename EnumType, typename = std::enable_if_t<!meta::isRegistered<EnumType>()>, typename = std::enable_if_t<meta::valuesRegistered<EnumType>()>>
BindingProperty serialize(const EnumType& obj);

template <typename Class, typename = std::enable_if_t<!meta::isRegistered<Class>()>, typename = std::enable_if_t<!meta::valuesRegistered<Class>()>, typename = void>
BindingProperty serialize(const Class & obj);

template <typename Class>
BindingProperty serialize_basic(const Class& obj);

template <typename T>
BindingProperty serialize_basic(const std::vector<T>& obj);

template <typename K, typename V>
BindingProperty serialize_basic(const std::unordered_map<K, V>& obj);

template <typename K, typename V>
BindingProperty serialize_basic(const std::map<K, V>& obj);


// ------------------------------------------------------------------
// |                                                                |
// |                        Deserialization                         |
// |                                                                |
// ------------------------------------------------------------------

template <typename Class>
Class deserialize(const BindingProperty& obj);

template <typename Class, typename = std::enable_if_t<meta::isRegistered<Class>()>>
void deserialize(Class & obj, const BindingProperty & object);

template <typename EnumType, typename = std::enable_if_t<!meta::isRegistered<EnumType>()>, typename = std::enable_if_t<meta::valuesRegistered<EnumType>()>>
void deserialize(EnumType & obj, const BindingProperty & object);

template <typename Class, typename = std::enable_if_t<!meta::isRegistered<Class>()>, typename = std::enable_if_t<!meta::valuesRegistered<Class>()>, typename = void>
void deserialize(Class & obj, const BindingProperty & object);

template <typename T>
void deserialize(std::optional<T>& obj, const BindingProperty& object);

template <typename T>
void deserialize(std::vector<T>& obj, const BindingProperty& object);

template <typename K, typename V>
void deserialize(std::unordered_map<K, V>& obj, const BindingProperty& object);

template <typename K, typename V>
void deserialize(std::map<K, V>& obj, const BindingProperty& object);

}; // namespace Binding

}; // namespace CubeWorld

#include "BindingProperty.h"