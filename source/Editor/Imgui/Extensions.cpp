// By Thomas Steinke

#include <imgui.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui_internal.h>

#include "Extensions.h"

namespace CubeWorld
{

namespace Editor
{

namespace ImGuiEx
{

namespace
{

bool TimelineBehavior(const ImRect& bb, ImGuiID id, double* v, const double v_min, const double v_max, std::vector<double> hooks, const char* format, ImRect* out_grab_bb)
{
   ImGuiContext& g = *GImGui;
   const ImGuiStyle& style = g.Style;

   const ImGuiAxis axis = ImGuiAxis_X;

   const float grab_padding = 2.0f;
   const float slider_sz = (bb.Max[axis] - bb.Min[axis]) - grab_padding * 2.0f;
   float grab_sz = ImMin(style.GrabMinSize, slider_sz);
   double v_range = (v_min < v_max ? v_max - v_min : v_min - v_max);
   const float slider_usable_sz = slider_sz - grab_sz;
   const float slider_usable_pos_min = bb.Min[axis] + grab_padding + grab_sz * 0.5f;
   const float slider_usable_pos_max = bb.Max[axis] - grab_padding - grab_sz * 0.5f;

   // For power curve sliders that cross over sign boundary we want the curve to be symmetric around 0.0f
   float linear_zero_pos = v_min < 0.0f ? 1.0f : 0.0f;

   // Process interacting with the slider
   bool value_changed = false;
   if (g.ActiveId == id)
   {
      bool set_new_value = false;
      float clicked_t = 0.0f;
      if (g.ActiveIdSource == ImGuiInputSource_Mouse)
      {
         if (!g.IO.MouseDown[0])
         {
            ImGui::ClearActiveID();
         }
         else if (slider_usable_sz > 0.0f)
         {
            const float mouse_abs_pos = g.IO.MousePos[axis];
            clicked_t = ImClamp((mouse_abs_pos - slider_usable_pos_min) / slider_usable_sz, 0.0f, 1.0f);
            set_new_value = true;
         }
      }
      else if (g.ActiveIdSource == ImGuiInputSource_Nav)
      {
         const ImVec2 delta2 = ImGui::GetNavInputAmount2d(ImGuiNavDirSourceFlags_Keyboard | ImGuiNavDirSourceFlags_PadDPad, ImGuiInputReadMode_RepeatFast, 0.0f, 0.0f);
         float delta = delta2.x;
         if (g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
         {
            ImGui::ClearActiveID();
         }
         else if (delta != 0.0f)
         {
            clicked_t = ImGui::SliderCalcRatioFromValueT<double, double>(ImGuiDataType_Double, *v, v_min, v_max, 1.0f, linear_zero_pos);
            const int decimal_precision = ImParseFormatPrecision(format, 3);
            if (decimal_precision > 0)
            {
               delta /= 100.0f;    // Gamepad/keyboard tweak speeds in % of slider bounds
               if (ImGui::IsNavInputDown(ImGuiNavInput_TweakSlow))
               {
                  delta /= 10.0f;
               }
            }
            else
            {
               if ((v_range >= -100.0f && v_range <= 100.0f) || ImGui::IsNavInputDown(ImGuiNavInput_TweakSlow))
               {
                  delta = ((delta < 0.0f) ? -1.0f : +1.0f) / (float)v_range; // Gamepad/keyboard tweak speeds in integer steps
               }
               else
               {
                  delta /= 100.0f;
               }
            }
            if (ImGui::IsNavInputDown(ImGuiNavInput_TweakFast))
            {
               delta *= 10.0f;
            }
            set_new_value = true;
            if ((clicked_t >= 1.0f && delta > 0.0f) || (clicked_t <= 0.0f && delta < 0.0f)) // This is to avoid applying the saturation when already past the limits
            {
               set_new_value = false;
            }
            else
            {
               clicked_t = ImSaturate(clicked_t + delta);
            }
         }
      }

      if (set_new_value)
      {
         double v_new = ImLerp(v_min, v_max, clicked_t);

         // Snap to checkpoints
         // Use 8px of the slider size for snapping
         constexpr double SNAP_SIZE = 8.0;
         if (slider_usable_sz > SNAP_SIZE)
         {
            double snap_size = v_range * (SNAP_SIZE / slider_usable_sz);
            for (const double& hook : hooks)
            {
               if (std::abs(v_new - hook) < snap_size)
               {
                  v_new = hook;
                  break;
               }
            }
         }

         // Round to user desired precision based on format string
         v_new = ImGui::RoundScalarWithFormatT<double, double>(format, ImGuiDataType_Double, v_new);

         // Apply result
         if (*v != v_new)
         {
            *v = v_new;
            value_changed = true;
         }
      }
   }

   if (slider_sz < 1.0f)
   {
      *out_grab_bb = ImRect(bb.Min, bb.Min);
   }
   else
   {
      // Output grab position so it can be displayed by the caller
      float grab_t = ImGui::SliderCalcRatioFromValueT<double, double>(ImGuiDataType_Double, *v, v_min, v_max, 1.0f, linear_zero_pos);
      const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
      *out_grab_bb = ImRect(grab_pos - grab_sz * 0.5f, bb.Min.y + grab_padding, grab_pos + grab_sz * 0.5f, bb.Max.y - grab_padding);
   }

   return value_changed;
}

}; // anonymous namespace

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

bool Timeline(const std::string& label, double* time, double max, const std::vector<double>& keyframes, const char* format)
{
   // Similar to ImGui::SliderFloat, but with keyframe snapping
   ImGuiWindow* window = ImGui::GetCurrentWindow();
   if (window->SkipItems)
      return false;

   ImGuiContext& g = *GImGui;
   const ImGuiStyle& style = g.Style;
   const ImGuiID id = window->GetID(label.c_str());
   const float w = ImGui::CalcItemWidth();

   const ImVec2 label_size = ImGui::CalcTextSize(label.c_str(), nullptr, true);
   const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
   const ImRect total_bb(frame_bb.Min, frame_bb.Max);

   ImGui::ItemSize(total_bb, style.FramePadding.y);

   // Default format string when passing nullptr
   if (format == nullptr)
   {
      format = "0.2f";
   }

   // Tabbing or CTRL-clicking on Slider turns it into an input box
   const bool hovered = ImGui::ItemHoverable(frame_bb, id);
   bool temp_input_is_active = ImGui::TempInputTextIsActive(id);
   bool temp_input_start = false;
   if (!temp_input_is_active)
   {
      const bool focus_requested = ImGui::FocusableItemRegister(window, id);
      const bool clicked = (hovered && g.IO.MouseClicked[0]);
      if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
      {
         ImGui::SetActiveID(id, window);
         ImGui::SetFocusID(id, window);
         ImGui::FocusWindow(window);
         g.ActiveIdAllowNavDirFlags = (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
         if (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id)
         {
            temp_input_start = true;
            ImGui::FocusableItemUnregister(window);
         }
      }
   }
   if (temp_input_is_active || temp_input_start)
   {
      return ImGui::TempInputTextScalar(frame_bb, id, label.c_str(), ImGuiDataType_Double, time, format);
   }

   // Draw frame
   const ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
   ImGui::RenderNavHighlight(frame_bb, id);
   ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

   // Render keyframes
   for (const double& keyframe : keyframes)
   {
      const ImU32 keyframe_col = ImGui::GetColorU32(ImVec4(0.439f, 0.322f, 0.122f, 1));
      const ImRect keyframe_bb(frame_bb.Min, frame_bb.Max);
      const float percent = (float)(keyframe / max);
      const float padding = 2.0f + ImGui::GetStyle().GrabMinSize / 2.0f;
      const float size = 2.0f;
      const float x_center = frame_bb.Min.x + padding + percent * (frame_bb.Max.x - frame_bb.Min.x - 2.0f * padding);
      ImGui::RenderFrame(ImVec2(x_center - size, frame_bb.Min.y), ImVec2(x_center + size, frame_bb.Max.y), keyframe_col, true, g.Style.FrameRounding);
   }

   // Slider behavior
   ImRect grab_bb;
   const bool value_changed = TimelineBehavior(frame_bb, id, time, 0.0, max, keyframes, format, &grab_bb);
   if (value_changed)
   {
      ImGui::MarkItemEdited(id);
   }

   // Render grab
   if (grab_bb.Max.x > grab_bb.Min.x)
   {
      window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);
   }

   // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
   char value_buf[64];
   const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), ImGuiDataType_Double, time, format);
   ImGui::RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f,0.5f));

   // TODO take the whole row
   if (label_size.x > 0.0f)
   {
      ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label.c_str());
   }

   IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
   return value_changed;
}

}; // namespace ImGuiEx

}; // namespace Editor

}; // namespace CubeWorld
