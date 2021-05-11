//
// WorldGenerator - A tool for iterating on and viewing world generation
//

#include <algorithm>
#include <cmath>

#include <RGBDesignPatterns/CommandStack.h>
#include <RGBLogger/Logger.h>
#include <RGBLogger/StdoutLogger.h>
#include <RGBLogger/DebugLogger.h>
#include <RGBSettings/SettingsProvider.h>

#include <Engine/Core/Input.h>
#include <Engine/Core/StateManager.h>
#include <Engine/Core/Timer.h>
#include <Engine/Core/Window.h>
#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include <Shared/Imgui/Context.h>
#include <Shared/Imgui/StateWindow.h>

#include "States/MainState.h"
#include "States/RenderTestState.h"
#include "Main.h"

using namespace CubeWorld;
using namespace Engine;

const double FRAMES_PER_SEC = 60.0;
const double SEC_PER_FRAME = (1 / FRAMES_PER_SEC);

int main(int argc, char **argv)
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
   SettingsProvider::Instance().SetLocalPath("WorldGenerator");

   // Setup main window
   Window::Options windowOptions;
   windowOptions.title = "World Generator";
   windowOptions.fullscreen = false;
   windowOptions.width = 1600;
   windowOptions.height = 900;
   windowOptions.lockCursor = false;
   /*
   windowOptions.r = 0.529f;
   windowOptions.g = 0.808;
   windowOptions.b = 0.922;
   */
   Window &window = Window::Instance();
   if (auto result = window.Initialize(windowOptions); !result)
   {
      LOG_ERROR("Failed creating window: %s", result.Failure().GetMessage());
      return 1;
   }

   // Set up imgui
   Editor::ImguiContext imgui(window);

   // Configure Debug helper
   DebugHelper &debug = DebugHelper::Instance();
   debug.SetBounds(&window);

   // FPS clock
   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug.RegisterMetric("FPS", [&clock]() -> std::string {
      return FormatString("%.1f", std::round(1.0 / clock.Average()));
   });

   // ESC closes the app.
   window.AddCallback(GLFW_KEY_ESCAPE, [&](int, int, int) {
       window.SetShouldClose(true);
   }).release();

   // Create state manager and initial state.
   Engine::StateManager& stateManager = Engine::StateManager::Instance();
   //stateManager.SetState(std::make_unique<MainState>(window));

   StateWindow stateWindow(window, 600, 400, nullptr);

   std::unique_ptr<MainState> state{ new MainState(&window, stateWindow) };
   state->SetParent(&stateWindow);
   state->TransformParentEvents<MouseDownEvent>(&stateWindow);
   state->TransformParentEvents<MouseUpEvent>(&stateWindow);
   state->TransformParentEvents<MouseClickEvent>(&stateWindow);
   stateWindow.SetState(std::move(state));

   window.OnMouseDown([&](int button, double x, double y) {
       x *= window.GetWidth();
       y *= window.GetHeight();
       stateWindow.Emit<MouseDownEvent>(button, x, y);
   });
   window.OnMouseUp([&](int button, double x, double y) {
       x *= window.GetWidth();
       y *= window.GetHeight();
       stateWindow.Emit<MouseUpEvent>(button, x, y);
   });
   window.OnClick([&](int button, double x, double y) {
       x *= window.GetWidth();
       y *= window.GetHeight();
       stateWindow.Emit<MouseClickEvent>(button, x, y);
   });

   do
   {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         // Basic prep
         window.Clear();
         window.Update();

         TIMEDELTA dt = std::min(elapsed, SEC_PER_FRAME);

         imgui.StartFrame(dt);

         ImGui::SetNextWindowPos(ImVec2(250, 20), ImGuiCond_FirstUseEver);
         ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
         ImGui::Begin("World");
         ImVec2 space = ImGui::GetContentRegionAvail();
         if (space.y > 0)
         {
             ImVec2 pos = ImGui::GetCursorScreenPos();
             glm::tvec2<double> corrected = stateWindow.CorrectYCoordinate({
                 (double)pos.x,
                 (double)pos.y + stateWindow.GetHeight()
                 });
             stateWindow.SetPosition(corrected);
             stateWindow.SetSize(space);
             ImGui::ImageButton(
                 (ImTextureID)(intptr_t)stateWindow.GetFramebuffer().GetTexture(),
                 ImVec2((float)stateWindow.GetWidth(), (float)stateWindow.GetHeight()),
                 ImVec2(0, 1),
                 ImVec2(1, 0),
                 0
             );
         }

         ImGui::End();

         stateWindow.Update(dt);

         // Render debug stuff
         {
             debug.Update(true);
             debug.Render(true);
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

   stateManager.Shutdown();
   return 0;
}
