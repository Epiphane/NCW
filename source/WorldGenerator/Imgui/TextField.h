// By Thomas Steinke

#pragma once

#include <string>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace CubeWorld
{

namespace Editor
{

//
// Represents the combination of ImGui::InputText with CommandStack manipulation needs
//
class TextField
{
public:
   TextField() {};

public:
   bool Update(const std::string& label, std::string& value)
   {
      ImGui::InputText(label.c_str(), &value);
      if (ImGui::IsItemActivated())
      {
         initialValue = value;
      }

      return ImGui::IsItemDeactivatedAfterEdit();
   }

   const std::string& GetLastValue() const { return initialValue; }

private:
   std::string initialValue;
};

}; // namespace Editor

}; // namespace CubeWorld
