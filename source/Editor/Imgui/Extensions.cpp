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

bool SliderDouble(const std::string& label, double* v, double v_min, double v_max, const char* format, double power)
{
   float val = (float)*v;
   bool result = ImGui::SliderFloat(label.c_str(), &val, (float)v_min, (float)v_max, format, (float)power);
   *v = (double)val;
   return result;
}

}; // namespace ImGuiEx

}; // namespace Editor

}; // namespace CubeWorld
