#pragma once

/*

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <Meta.h>

#include "BindingProperty.h"

namespace meta
{

using CubeWorld::BindingProperty;

// ------------------------------------------------------------------
// |                                                                |
// |                         Serialization                          |
// |                                                                |
// ------------------------------------------------------------------

template <typename Class, typename = std::enable_if_t<meta::isRegistered<Class>()>>
BindingProperty serialize(const Class& obj);

template <typename Class, typename = std::enable_if_t <!meta::isRegistered<Class>()>, typename = void>
BindingProperty serialize(const Class& obj);

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

template <typename Class, typename = std::enable_if_t<meta::isRegistered<Class>()>>
void deserialize(Class& obj, const BindingProperty& object);

template <typename Class, typename = std::enable_if_t<!meta::isRegistered<Class>()>, typename = void>
void deserialize(Class & obj, const BindingProperty& object);

template <typename T>
void deserialize(std::vector<T>& obj, const BindingProperty& object);

template <typename K, typename V>
void deserialize(std::unordered_map<K, V>& obj, const BindingProperty& object);

template <typename K, typename V>
void deserialize(std::map<K, V>& obj, const BindingProperty& object);

}

#include "BindingPropertyMeta.inl"

*/