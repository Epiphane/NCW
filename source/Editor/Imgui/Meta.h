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

//
// Explicitly support each data type.
//
bool Draw(const std::string& label, std::string& val);
bool Draw(const std::string& label, BindingProperty& val);
bool Draw(const std::string& label, uint32_t& val);
bool Draw(const std::string& label, double& val);
bool Draw(const std::string& label, float& val);

template <typename EnumType, typename = std::enable_if_t<meta::valuesRegistered<EnumType>()>, typename = void>
bool Draw(const std::string& label, EnumType& obj);

template <typename Class, typename = std::enable_if_t<meta::isRegistered<Class>()>>
bool Draw(const std::string & label, Class& obj);

template <typename EnumType, typename, typename>
bool Draw(const std::string& label, EnumType& obj)
{
   std::string val = Binding::serialize< EnumType>(obj).GetStringValue();
   EnumType newVal = obj;

   if (ImGui::BeginCombo(label.c_str(), val.c_str()))
   {
      meta::doForAllValues<EnumType>(
         [&](auto& value)
         {
            bool isSelected = (value.getValue() == obj);
            if (ImGui::Selectable(value.getName().c_str(), isSelected))
            {
               newVal = value.getValue();
            }
         }
      );

      ImGui::EndCombo();
   }

   if (newVal != obj)
   {
      CommandStack::Instance().Do<SetValueCommand<EnumType>>(obj, newVal);
      return true;
   }

   return false;
}

template <typename Class, typename>
bool Draw(const std::string& label, Class& obj)
{
   bool changed = false;

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

   std::string labelSuffix = "##" + label;
   if (label.empty())
   {
      labelSuffix += '_';
   }

   meta::doForAllMembers<Class>(
      [&](auto& member)
      {
         std::string sublabel = std::string(member.getName()) + labelSuffix;

         if (member.hasSetter()) {
            // I dunno man
         }
         else if (member.canGetRef()) {
            changed |= Draw(sublabel, member.getRef(obj));
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
