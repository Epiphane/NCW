// By Thomas Steinke

#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include <imgui.h>
#include <RGBDesignPatterns/Command.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Graphics/TextureManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>
#include <RGBLogger/Logger.h>
#include <Engine/UI/Binding.h>
#include <Engine/UI/UIElement.h>
#include <Shared/UI/Image.h>

namespace CubeWorld
{

namespace Editor
{

//
// Represents the combination of ImGui::DragFloat3 and all associated
// CommandStack manipulation needs
//
class ScrubberVec3
{
public:
   ScrubberVec3(std::function<void(glm::vec3, glm::vec3)> onChange)
      : onChange(onChange)
   {};

public:
   void Update(const std::string& label, glm::vec3& value, float sensitivity = 1.0f)
   {
      ImGui::DragFloat3(label.c_str(), &value.x, sensitivity);
      if (ImGui::IsItemActivated())
      {
         initialValue = value;
      }

      if (ImGui::IsItemDeactivatedAfterEdit())
      {
         onChange(initialValue, value);
      }
   }

private:
   std::function<void(glm::vec3, glm::vec3)> onChange;
   glm::vec3 initialValue;
};

}; // namespace Editor

}; // namespace CubeWorld
