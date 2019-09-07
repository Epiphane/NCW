// By Thomas Steinke
// Useful ImGui extensions

#pragma once

#include <imgui.h>
#include <string>

namespace CubeWorld
{

namespace Editor
{

namespace ImGuiEx
{

bool RedButton(const std::string& label, const ImVec2& size = ImVec2(0,0));
bool Button(bool red, const std::string& label, const ImVec2& size = ImVec2(0,0));
bool SliderDouble(const std::string& label, double* v, double v_min = 0.0, double v_max = 0.0, const char* format = "%.3f", double power = 1.0);     // If v_min >= v_max we have no bound

}; // namespace ImGuiEx

}; // namespace Editor

}; // namespace CubeWorld
