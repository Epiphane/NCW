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

namespace CubeWorld
{

namespace Editor
{

template<typename T, typename C>
struct ScrubProvider {
   static bool Drag(const std::string& label, T& v, float speed) = 0;
   static bool Slide(const std::string& label, T& v, C min, C max) = 0;
};

template<> struct ScrubProvider<glm::vec3, float>
{
   static bool Drag(const std::string& label, glm::vec3& v, float speed)
   {
      return ImGui::DragFloat3(label.c_str(), &v.x, speed);
   }

   static bool Slide(const std::string& label, glm::vec3& v, float min, float max)
   {
      return ImGui::SliderFloat3(label.c_str(), &v.x, min, max);
   }
};

template<> struct ScrubProvider<double, double>
{
   static bool Drag(const std::string& label, double& v, float speed)
   {
      float val = (float)v;
      bool result = ImGui::DragFloat(label.c_str(), &val, speed);
      v = (double)val;
      return result;
   }


   static bool Slide(const std::string& label, double& v, double min, double max)
   {
      float val = (float)v;
      bool result = ImGui::SliderFloat(label.c_str(), &val, (float)min, (float)max);
      v = (double)val;
      return result;
   }
};

//
// Represents the combination of ImGui::DragFloat3 and all associated
// CommandStack manipulation needs
//
template<typename T, typename C = T>
class Scrubber
{
public:
   Scrubber() {};

public:
   bool Drag(const std::string& label, T& value, float speed = 1.0f)
   {
      ScrubProvider<T, C>::Drag(label.c_str(), value, speed);
      if (ImGui::IsItemActivated())
      {
         initialValue = value;
      }

      return ImGui::IsItemDeactivatedAfterEdit();
   }

   bool Slide(const std::string& label, T& value, C min, C max)
   {
      ScrubProvider<T, C>::Slide(label.c_str(), value, min, max);
      if (ImGui::IsItemActivated())
      {
         initialValue = value;
      }

      return ImGui::IsItemDeactivatedAfterEdit();
   }

   T GetLastValue() { return initialValue; }

private:
   T initialValue;
};

}; // namespace Editor

}; // namespace CubeWorld
