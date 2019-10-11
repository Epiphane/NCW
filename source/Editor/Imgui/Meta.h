// By Thomas Steinke

#pragma once

#include <string>
#include <imgui.h>
#include <Meta.h>
#include <RGBBinding/BindingProperty.h>
#include <RGBDesignPatterns/Command.h>
#include <RGBMeta/Value.h>

namespace CubeWorld
{

namespace Editor
{

namespace Imgui
{

template<typename T>
struct SetValueCommand : public Command
{
   SetValueCommand(T& location, T value) : location(location), value(value) {};

   void Do() override
   {
      T temp = std::move(location);
      location = std::move(value);
      value = std::move(temp);
   }
   void Undo() override { Do(); }

   T& location;
   T value;
};

template<typename T>
struct SetIndexCommand : public Command
{
   SetIndexCommand(std::vector<T>& location, size_t index, T value)
      : location(location)
      , index(index)
      , value(value)
   {};

   void Do() override
   {
      T temp = std::move(location.at(index));
      location[index] = std::move(value);
      value = std::move(temp);
   }
   void Undo() override { Do(); }

   std::vector<T>& location;
   size_t index;
   T value;
};

//
// Explicitly support each data type.
//
bool Draw(const std::string& label, std::string& val, bool addToStack = true);
bool Draw(const std::string& label, BindingProperty& val, bool addToStack = true);
bool Draw(const std::string& label, glm::vec3& val, bool addToStack = true);
bool Draw(const std::string& label, uint32_t& val, bool addToStack = true);
bool Draw(const std::string& label, double& val, bool addToStack = true);
bool Draw(const std::string& label, float& val, bool addToStack = true);
bool Draw(const std::string& label, bool& val, bool addToStack = true);

template<typename Class>
bool Draw(const std::string& label, std::vector<Class>& val, bool addToStack = true)
{
   if (!label.empty() && label[0] != '#')
   {
      size_t pos = label.find('#');
      if (pos == std::string::npos)
      {
         ImGui::Text("%s", label.c_str());
      }
      else
      {
         ImGui::Text("%s", label.substr(0, pos).c_str());
      }
   }

   bool modified = false;

   size_t index = 0;
   for (Class& item : val)
   {
      if (ImGui::TreeNode(FormatString("Element {num}##{label}", index, label).c_str()))
      {
         modified |= Draw(FormatString("##{label}##{num}", label, index), item, addToStack);
         ImGui::TreePop();
      }
      index++;
   }

   return modified;
}

template <typename Class,
   typename = std::enable_if_t<!meta::isRegistered<Class>()>,
   typename = std::enable_if_t<!meta::valuesRegistered<Class>()>,
   typename = void>
bool Draw(const std::string&, Class&, bool = true) { return false; }

template <typename EnumType, typename = std::enable_if_t<meta::valuesRegistered<EnumType>()>, typename = void>
bool Draw(const std::string& label, EnumType& obj, bool addToStack = true);

template <typename Class, typename = std::enable_if_t<meta::isRegistered<Class>()>>
bool Draw(const std::string & label, Class& obj, bool addToStack = true);

template <typename EnumType, typename, typename>
bool Draw(const std::string& label, EnumType& obj, bool addToStack)
{
   std::string val = meta::getName<EnumType>(obj);
   EnumType newVal = obj;

   if (ImGui::BeginCombo(label.c_str(), val.c_str()))
   {
      meta::doForAllValues<EnumType>(
         [&](const auto& item)
         {
            bool isSelected = (item.getValue() == obj);
            if (ImGui::Selectable(item.getName().c_str(), isSelected))
            {
               newVal = item.getValue();
            }
         }
      );

      ImGui::EndCombo();
   }

   if (newVal != obj)
   {
      if (addToStack)
      {
         CommandStack::Instance().Do<SetValueCommand<EnumType>>(obj, newVal);
      }
      return true;
   }

   return false;
}

template <typename Class, typename>
bool Draw(const std::string& label, Class& obj, bool addToStack)
{
   bool changed = false;

   std::string labelSuffix;
   if (!label.empty() && label[0] != '#')
   {
      size_t pos = label.find('#');
      if (pos == std::string::npos)
      {
         ImGui::Text("%s", label.c_str());
      }
      else
      {
         ImGui::Text("%s", label.substr(0, pos).c_str());
      }
      labelSuffix = "##" + label;
   }
   else
   {
      labelSuffix = "##_" + label;
   }

   meta::doForAllMembers<Class>(
      [&](auto& member)
      {
         if (!member.enabled(obj))
         {
            return;
         }

         std::string sublabel = std::string(member.getName()) + labelSuffix;

         if (member.hasSetter()) {
            auto copy = member.getCopy(obj);
            changed |= Draw(sublabel, copy, addToStack);
            member.set(obj, copy);
         }
         else if (member.canGetRef()) {
            changed |= Draw(sublabel, member.getRef(obj), addToStack);
         }
         else {
            assert(false && "can't deserialize member because it's read only");
         }
      }
   );

   return changed;
}


}; // namespace Imgui

}; // namespace Editor

}; // namespace CubeWorld
