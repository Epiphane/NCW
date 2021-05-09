// By Thomas Steinke
// Useful ImGui extensions

#pragma once

#include <imgui.h>
#include <string>
#include <vector>

namespace CubeWorld
{

namespace ImGuiEx
{

bool RedButton(const std::string& label, const ImVec2& size = ImVec2(0,0));
bool Button(bool red, const std::string& label, const ImVec2& size = ImVec2(0,0));
bool SliderDouble(const std::string& label, double* v, double v_min = 0.0, double v_max = 0.0, const char* format = "%.3f", double power = 1.0);     // If v_min >= v_max we have no bound
bool Timeline(const std::string& label, double* time, double max, bool snap, const std::vector<double>& keyframes, const char* format = "%.2f");

}; // namespace ImGuiEx

}; // namespace CubeWorld
