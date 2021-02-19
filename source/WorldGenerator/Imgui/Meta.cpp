// By Thomas Steinke

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <RGBDesignPatterns/CommandStack.h>

#include "Meta.h"

namespace CubeWorld
{

namespace Editor
{

namespace Imgui
{

bool Draw(const std::string& label, std::string& value, bool addToStack)
{
   static std::string initialValue;

   ImGui::InputText(label.c_str(), &value);
   if (ImGui::IsItemActivated())
   {
      initialValue = value;
   }

   if (ImGui::IsItemDeactivatedAfterEdit())
   {
      if (addToStack)
      {
         CommandStack::Instance().Emplace<SetValueCommand<std::string>>(value, initialValue);
      }
      return true;
   }

   return false;
}

std::pair<bool, bool> DrawInternal(const std::string& label, BindingProperty& value)
{
   BindingProperty::Type type = value.GetType();

   const BindingProperty::Type types[] = {
      BindingProperty::Type::kNullType,
      BindingProperty::Type::kTrueType,
      BindingProperty::Type::kFalseType,
      BindingProperty::Type::kNumberType,
      BindingProperty::Type::kStringType,
      BindingProperty::Type::kObjectType,
      BindingProperty::Type::kArrayType,
   };

   std::pair<bool, bool> result{false, false};
   if (ImGui::BeginCombo(("##type" + label).c_str(), BindingProperty::TypeToString(type).c_str()))
   {
      result.first = true;
      for (const BindingProperty::Type t : types)
      {
         bool isSelected = (t == type);
         if (ImGui::Selectable(BindingProperty::TypeToString(t).c_str(), isSelected) && !isSelected)
         {
            value = BindingProperty(t);
            result.second = true;
         }
      }
      ImGui::EndCombo();
   }

   std::string keyLabel = "##key" + label;
   std::string valueLabel = "##value" + label;

   type = value.GetType();
   switch (type)
   {
   case BindingProperty::Type::kNullType:
   case BindingProperty::Type::kTrueType:
   case BindingProperty::Type::kFalseType:
      break;

   case BindingProperty::Type::kNumberType:
   {
      float floatVal = value.GetFloatValue();
      ImGui::InputFloat(valueLabel.c_str(), &floatVal);
      value.SetFloat(floatVal);

      result.first |= ImGui::IsItemActivated();
      result.second |= ImGui::IsItemDeactivatedAfterEdit();
      break;
   }

   case BindingProperty::Type::kStringType:
   {
      std::string val = value.GetStringValue();
      ImGui::InputText(valueLabel.c_str(), &val);
      value.SetString(val);

      result.first |= ImGui::IsItemActivated();
      result.second |= ImGui::IsItemDeactivatedAfterEdit();
      break;
   }

   case BindingProperty::Type::kObjectType:
   {
      int nElements = (int)value.GetSize();
      ImGui::InputInt(("##size" + label).c_str(), &nElements);

      if (ImGui::IsItemDeactivatedAfterEdit() && nElements >= 0)
      {
         while (value.GetSize() > size_t(nElements))
         {
            value.PopBack();
         }
         while (value.GetSize() < size_t(nElements))
         {
            value.Set(FormatString("Element {num}", value.GetSize()), BindingProperty(0));
         }
      }

      size_t index = 0;
      for (auto& keyval : value.object())
      {
         if (ImGui::TreeNode(FormatString("Element {num}{label}", index++, label).c_str()))
         {
            ImGui::InputText(keyLabel.c_str(), &keyval.key);

            result.first |= ImGui::IsItemActivated();
            result.second |= ImGui::IsItemDeactivatedAfterEdit();

            std::pair<bool, bool> valueResult = DrawInternal(label + "##" + keyval.key, keyval.value);
            result.first |= valueResult.first;
            result.second |= valueResult.second;

            ImGui::TreePop();
         }
      }

      break;
   }

   case BindingProperty::Type::kArrayType:
   {
      int nElements = (int)value.GetSize();
      ImGui::InputInt(("##size" + label).c_str(), &nElements);

      if (ImGui::IsItemDeactivatedAfterEdit() && nElements >= 0)
      {
         while (value.GetSize() > size_t(nElements))
         {
            value.PopBack();
         }
         while (value.GetSize() < size_t(nElements))
         {
            value.PushBack(BindingProperty{});
         }
      }

      if (value.IsVec3())
      {
         glm::vec3 val = value.GetVec3();

         // Cover all possible types
         ImGui::ColorEdit3(valueLabel.c_str(), &val.x);
         result.first |= ImGui::IsItemActivated();
         result.second |= ImGui::IsItemDeactivatedAfterEdit();
         ImGui::DragFloat3(valueLabel.c_str(), &val.x);
         result.first |= ImGui::IsItemActivated();
         result.second |= ImGui::IsItemDeactivatedAfterEdit();

         value = val;
      }
      else if (value.IsVec4())
      {
         glm::vec4 val = value.GetVec4();

         // Cover all possible types
         ImGui::ColorEdit4(valueLabel.c_str(), &val.x);
         result.first |= ImGui::IsItemActivated();
         result.second |= ImGui::IsItemDeactivatedAfterEdit();
         ImGui::DragFloat4(valueLabel.c_str(), &val.x);
         result.first |= ImGui::IsItemActivated();
         result.second |= ImGui::IsItemDeactivatedAfterEdit();

         value = val;
      }
      else
      {
         size_t index = 0;
         for (auto& elem : value)
         {
            if (ImGui::TreeNode(FormatString("Element {num}{label}", index, label).c_str()))
            {
               std::pair<bool, bool> valueResult = DrawInternal(FormatString(label + "##{num}", index), elem);
               result.first |= valueResult.first;
               result.second |= valueResult.second;

               ImGui::TreePop();
            }
            index++;
         }
      }

      break;
   }

   }

   return result;
}

bool Draw(const std::string& label, BindingProperty& value, bool addToStack)
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

   static BindingProperty initialValue;

   auto [activated, edited] = DrawInternal("##" + label, value);
   if (activated)
   {
      initialValue = value;
   }

   if (edited && addToStack)
   {
      CommandStack::Instance().Emplace<SetValueCommand<BindingProperty>>(value, initialValue);
   }

   return edited;
}

bool Draw(const std::string& label, glm::vec3& value, bool addToStack)
{
   static glm::vec3 initialValue;
   ImGui::InputFloat3(label.c_str(), &value.x);

   if (ImGui::IsItemActivated())
   {
      initialValue = value;
   }

   if (ImGui::IsItemDeactivatedAfterEdit())
   {
      if (addToStack)
      {
         CommandStack::Instance().Emplace<SetValueCommand<glm::vec3>>(value, initialValue);
      }
      return true;
   }

   return false;
}

bool Draw(const std::string& label, uint32_t& value, bool addToStack)
{
   static uint32_t initialValue;

   int step = 1;
   int step_fast = 100;
   ImGui::InputScalar(label.c_str(), ImGuiDataType_U32, (void*)& value, (void*)&step, (void*)&step_fast, "%d", 0);
   if (ImGui::IsItemActivated())
   {
      initialValue = value;
   }

   if (ImGui::IsItemDeactivatedAfterEdit())
   {
      if (addToStack)
      {
         CommandStack::Instance().Emplace<SetValueCommand<uint32_t>>(value, initialValue);
      }
      return true;
   }

   return false;
}

bool Draw(const std::string& label, double& value, bool addToStack)
{
   static double initialValue;

   ImGui::InputDouble(label.c_str(), &value);
   if (ImGui::IsItemActivated())
   {
      initialValue = value;
   }

   if (ImGui::IsItemDeactivatedAfterEdit())
   {
      if (addToStack)
      {
         CommandStack::Instance().Emplace<SetValueCommand<double>>(value, initialValue);
      }
      return true;
   }

   return false;
}

bool Draw(const std::string& label, float& value, bool addToStack)
{
   static float initialValue;

   ImGui::InputFloat(label.c_str(), &value);
   if (ImGui::IsItemActivated())
   {
      initialValue = value;
   }

   if (ImGui::IsItemDeactivatedAfterEdit())
   {
      if (addToStack)
      {
         CommandStack::Instance().Emplace<SetValueCommand<float>>(value, initialValue);
      }
      return true;
   }

   return false;
}

bool Draw(const std::string& label, bool& value, bool addToStack)
{
   if (ImGui::Checkbox(label.c_str(), &value))
   {
      if (addToStack)
      {
         CommandStack::Instance().Emplace<SetValueCommand<bool>>(value, !value);
      }
      return true;
   }

   return false;
}

}; // namespace Imgui

}; // namespace Editor

}; // namespace CubeWorld
