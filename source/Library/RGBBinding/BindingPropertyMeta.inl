#pragma once

#include "BindingPropertyMeta.h"

namespace CubeWorld
{

// ------------------------------------------------------------------
// |                                                                |
// |                         Serialization                          |
// |                                                                |
// ------------------------------------------------------------------

template <typename Class, typename>
BindingProperty serialize(const Class& obj)
{
   BindingProperty value;
   meta::doForAllMembers<Class>(
      [&obj, &value](auto& member)
      {
         auto& valueName = value[member.getName()];
         if (member.canGetConstRef()) {
            valueName = member.get(obj);
         }
         else if (member.hasGetter()) {
            valueName = member.getCopy(obj); // passing copy as const ref, it's okay
         }
      }
   );
   return value;
}

template <typename Class>
BindingProperty serialize_basic(const Class& obj)
{
   return BindingProperty(obj);
}

template <typename T>
BindingProperty serialize_basic(const std::vector<T>& obj)
{
   BindingProperty value;
   for (const auto& elem : obj)
   {
      value.PushBack(elem);
   }
   return value;
}

template <typename K, typename V>
BindingProperty serialize_basic(const std::unordered_map<K, V>& obj)
{
   BindingProperty value;
   for (const auto& pair : obj)
   {
      value[pair.first] = pair.second;
   }
   return value;
}

// ------------------------------------------------------------------
// |                                                                |
// |                        Deserialization                         |
// |                                                                |
// ------------------------------------------------------------------

template <typename Class>
Class deserialize(const BindingProperty& obj)
{
   Class c;
   deserialize(c, obj);
   return c;
}

template <typename Class, typename>
void deserialize(Class& obj, const BindingProperty& object)
{
   if (!object.IsObject())
   {
      return;
   }

   meta::doForAllMembers<Class>(
      [&obj, &object](auto& member)
      {
         const BindingProperty& objName = object[member.getName()];
         if (!objName.IsNull()) {
            using MemberT = meta::get_member_type<decltype(member)>;
            if (member.hasSetter()) {
               member.set(obj, objName.template Get<MemberT>());
            }
            else if (member.canGetRef()) {
               member.getRef(obj) = objName.template Get<MemberT>();
            }
            else {
               assert(false && "can't deserialize member because it's read only");
            }
         }
      }
   );
}

template <typename T>
void deserialize(std::vector<T>& obj, const BindingProperty& object)
{
   obj.reserve(object.GetSize());
   for (const BindingProperty& elem : object) {
      obj.push_back(elem.Get<T>()); // push rvalue
   }
}

template <typename K, typename V>
void deserialize(std::map<K, V>& obj, const BindingProperty& object)
{
   for (const auto& [key, value] : object.pairs())
   {
      obj.emplace(key.Get<K>(), value.Get<V>());
   }
}

template <typename K, typename V>
void deserialize(std::unordered_map<K, V>& obj, const BindingProperty& object)
{
   for (const auto& [key, value] : object.pairs())
   {
      obj.emplace(key.Get<K>(), value.Get<V>());
   }
}

}; // namespace CubeWorld
