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

      if (arg == "--shader-root")
      {
          Asset::SetShaderRoot(argv[argi++]);
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
   windowOptions.width = 1280;
   windowOptions.height = 760;
   windowOptions.lockCursor = false;
   windowOptions.r = 0.529f;
   windowOptions.g = 0.808;
   windowOptions.b = 0.922;
   Window &window = Window::Instance();
   if (auto result = window.Initialize(windowOptions); !result)
   {
      LOG_ERROR("Failed creating window: %s", result.Failure().GetMessage());
      return 1;
   }

   // Configure Debug helper
   DebugHelper &debug = DebugHelper::Instance();
   debug.SetBounds(&window);

   // FPS clock
   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug.RegisterMetric("FPS", [&clock]() -> std::string {
      return FormatString("%.1f", std::round(1.0 / clock.Average()));
   });

   // ESC closes the app.
   auto _ = window.AddCallback(GLFW_KEY_ESCAPE, [&](int, int, int) {
       window.SetShouldClose(true);
   });


   auto _2 = window.AddCallback(GLFW_KEY_T, [&](int, int, int) {
       window.SetMouseLock(!window.IsMouseLocked());
   });

   // Create state manager and initial state.
   Engine::StateManager& stateManager = Engine::StateManager::Instance();
   stateManager.SetState(new MainState(window));

   do
   {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         // Basic prep
         window.Clear();
         window.Update();

         TIMEDELTA dt = std::min(elapsed, SEC_PER_FRAME);
         stateManager.Update(dt);

         // Render debug stuff
         {
             debug.Update();
             debug.Render();
         }

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
