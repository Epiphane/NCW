//
// NotCubeWorld - An attempt to make something that looks like CubeWorld
//

#include <algorithm>
#include <cassert>
#include <cmath>
#include <GL/includes.h>
#if !CUBEWORLD_PLATFORM_WINDOWS
#include <libfswatch/c/libfswatch.h>
#endif

#include <RGBDesignPatterns/CommandStack.h>
#include <RGBLogger/Logger.h>
#include <RGBLogger/StdoutLogger.h>
#include <RGBLogger/DebugLogger.h>
#include <RGBSettings/SettingsProvider.h>

#include <Engine/Core/Input.h>
#include <Engine/Core/Timer.h>
#include <Engine/Core/Window.h>
#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include <Shared/UI/Swapper.h>

#include "AnimationStation/Editor.h"
#include "Command/Commands.h"
#include "ParticleSpace/Editor.h"
#include "ShaderLand/Editor.h"
#include "Skeletor/Editor.h"

#include <Shared/Imgui/Context.h>
#include "Main.h"

using namespace CubeWorld;
using namespace Engine;

const double FRAMES_PER_SEC = 60.0;
const double SEC_PER_FRAME = (1 / FRAMES_PER_SEC);

int main(int argc, char** argv)
{
   Asset::SetAssetRootDefault();

   // Parse arguments
   int argi = 0;
   while (argi < argc)
   {
      std::string arg(argv[argi++]);
      if (arg == "--asset-root")
      {
         Asset::SetAssetRoot(argv[argi++]);
      }
   }

   // Initialize and register loggers to VS debugger and stdout
   Logger::StdoutLogger::Instance();
   Logger::DebugLogger::Instance();

   // Set up settings location
   SettingsProvider::Instance().SetLocalPath("Editor");

   // Setup main window
   Window::Options windowOptions;
   windowOptions.title = "NCW Editor";
   windowOptions.fullscreen = false;
   windowOptions.width = 1920;
   windowOptions.height = 1080;
   windowOptions.lockCursor = false;
   Window& window = Window::Instance();
   if (auto result = window.Initialize(windowOptions); !result)
   {
      LOG_ERROR("Failed creating window: %s", result.Failure().GetMessage());
      return 1;
   }

   glEnable(GL_PRIMITIVE_RESTART);
   glPrimitiveRestartIndex(GLuint(-1));

   // Setup file watching library
#if !CUBEWORLD_PLATFORM_WINDOWS
   fsw_init_library();
   fsw_set_verbose(true);
#endif

   // Swaps between the different editors
   UI::Swapper windowContent;

   Editor::AnimationStation::Editor* animationStation = nullptr;
   Editor::Skeletor::Editor* skeletor = nullptr;
   Editor::ParticleSpace::Editor* particleSpace = nullptr;

   Editor::ImguiContext imgui(window);

   // Create editors
   animationStation = windowContent.Add<Editor::AnimationStation::Editor>(window);
   skeletor = windowContent.Add<Editor::Skeletor::Editor>(window);
   particleSpace = windowContent.Add<Editor::ParticleSpace::Editor>(window);
   Editor::ShaderLand::Editor* shaderLand = windowContent.Add<Editor::ShaderLand::Editor>(window);

   // Configure Debug helper
   DebugHelper& debug = DebugHelper::Instance();

   // FPS clock
   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug.RegisterMetric("FPS", [&clock]() -> std::string {
      return FormatString("%.1f", std::round(1.0 / clock.Average()));
   });

   // Attach mouse events to state
   window.OnMouseDown([&](int button, double x, double y) {
      x *= window.GetWidth();
      y *= window.GetHeight();
      windowContent.GetCurrent()->Emit<MouseDownEvent>(button, x, y);
   });
   window.OnMouseUp([&](int button, double x, double y) {
      x *= window.GetWidth();
      y *= window.GetHeight();
      windowContent.GetCurrent()->Emit<MouseUpEvent>(button, x, y);
   });
   window.OnClick([&](int button, double x, double y) {
      x *= window.GetWidth();
      y *= window.GetHeight();
      windowContent.GetCurrent()->Emit<MouseClickEvent>(button, x, y);
   });

   // Save the pointers so that the callback doesn't get deregistered.
   auto _1 = window.AddCallback(Engine::Window::CtrlKey(GLFW_KEY_Z), [&](int, int, int) {
      CommandStack::Instance().Undo();
   });
   auto _2 = window.AddCallback({
      Engine::Window::CtrlShiftKey(GLFW_KEY_Z),
      Engine::Window::CtrlKey(GLFW_KEY_Y)
   }, [&](int, int, int) {
      CommandStack::Instance().Redo();
   });

   // Start in Animation Station
   const std::string& firstState = SettingsProvider::Instance().Get("main", "editor").GetStringValue("animation_station");
   (void)(firstState.c_str());
   if (firstState == "skeletor")
   {
      auto state = skeletor;
      state->Start();
      windowContent.Swap(state);
   }
   else if (firstState == "particle_space")
   {
       auto state = particleSpace;
       state->Start();
       windowContent.Swap(state);
   }
   else if (firstState == "shader_land")
   {
       auto state = shaderLand;
       state->Start();
       windowContent.Swap(state);
   }
   /*
   else if (firstState == "constrainer")
   {
      auto state = constrainer;
      state->Start();
      windowContent.Swap(state);
   }
   */
   else // if (firstState == "animation_station")
   {
      auto state = animationStation;
      state->Start();
      windowContent.Swap(state);
   }

   Timer<100> windowContentRender;
   auto _3 = debug.RegisterMetric("Editor Render time", [&windowContentRender]() -> std::string {
      return FormatString("%.1f", windowContentRender.Average());
   });

   Timer<100> uiUpdateTime;
   auto _4 = debug.RegisterMetric("UI UpdateRoot", [&uiUpdateTime]() -> std::string {
      return FormatString("%.1f", uiUpdateTime.Average());
   });

   uiUpdateTime.Reset();
   //windowContent.GetCurrent()->UpdateRoot();
   LOG_ALWAYS("Time spent updating initial root: {time}s", uiUpdateTime.Elapsed());

   do {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         TIMEDELTA dt = std::min(elapsed, SEC_PER_FRAME);

         imgui.StartFrame(dt);

         ImGui::SetNextWindowPos(ImVec2(25, 550), ImGuiCond_FirstUseEver);
         ImGui::SetNextWindowSize(ImVec2(200, 0));
         ImGui::Begin("Editors", nullptr, ImGuiWindowFlags_NoResize);

         ImVec2 space = ImGui::GetContentRegionAvail();
         if (ImGui::Button("Animation Station", ImVec2(space.x, 0)))
         {
            CommandStack::Instance().Do<Editor::NavigateCommand>(&windowContent, animationStation);
            SettingsProvider::Instance().Set("main", "editor", "animation_station");
            animationStation->Start();
         }

         if (ImGui::Button("Skeletor", ImVec2(space.x, 0)))
         {
            CommandStack::Instance().Do<Editor::NavigateCommand>(&windowContent, skeletor);
            SettingsProvider::Instance().Set("main", "editor", "skeletor");
            skeletor->Start();
         }

         if (ImGui::Button("Particle Space", ImVec2(space.x, 0)))
         {
            CommandStack::Instance().Do<Editor::NavigateCommand>(&windowContent, particleSpace);
            SettingsProvider::Instance().Set("main", "editor", "particle_space");
            particleSpace->Start();
         }

         if (ImGui::Button("Shader Land", ImVec2(space.x, 0)))
         {
             CommandStack::Instance().Do<Editor::NavigateCommand>(&windowContent, shaderLand);
             SettingsProvider::Instance().Set("main", "editor", "shader_land");
             shaderLand->Start();
         }

         if (ImGui::Button("Quit", ImVec2(space.x, 0)))
         {
             window.SetShouldClose(true);
         }

         ImGui::End();

         // Basic prep
         window.Clear();
         window.Update();

         glm::tvec2<double> pos = window.GetRawMousePosition();
         windowContent.GetCurrent()->Emit<MouseMoveEvent>(pos.x, pos.y);

         // Render game state
         {
            windowContentRender.Reset();
            uiUpdateTime.Reset();
            uiUpdateTime.Elapsed();
            windowContent.GetCurrent()->Update(dt);
            windowContentRender.Elapsed();
         }

         // Render debug stuff
         {
            debug.Update();
            debug.Render();
         }

         imgui.Render();

         // Swap buffers
         {
            window.SwapBuffers();
            glfwPollEvents();
         }
      }
   } // Check if the ESC key was pressed or the window was closed
   while (!window.ShouldClose());

   // We don't use the StateManager, so don't shut it down.

   return 0;
}
