// By Thomas Steinke

#include <imgui.h>

#include "Extensions.h"

namespace CubeWorld
{

namespace Editor
{

namespace ImGuiEx
{

bool RedButton(const std::string& label, const ImVec2& size)
{
   ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.98f, 0.25f, 0.25f, 0.40f));
   ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.98f, 0.25f, 0.25f, 0.40f));
   ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.98f, 0.05f, 0.05f, 0.40f));
   bool result = ImGui::Button(label.c_str(), size);
   ImGui::PopStyleColor(3);

   return result;
}

bool Button(bool red, const std::string& label, const ImVec2& size)
{
   if (red)
   {
      return RedButton(label, size);
   }
   return ImGui::Button(label.c_str(), size);
}

bool DragDouble(const std::string& label, double* v, double, double v_min, double v_max, const char* format, double power)
{
   float val = *v;
   bool result = ImGui::SliderFloat(label.c_str(), &val, v_min, v_max, format, power);
   *v = val;
   return result;
}

}; // namespace ImGuiEx

}; // namespace Editor

}; // namespace CubeWorld
