//
// NotCubeWorld - An attempt to make something that looks like CubeWorld
//

#include <RGBLogger/Logger.h>
#include <RGBLogger/StdoutLogger.h>
#include <RGBLogger/DebugLogger.h>
#include <Engine/Core/Input.h>
#include <Engine/Core/StateManager.h>
#include <Engine/Core/Timer.h>
#include <Engine/Core/Window.h>
#include <Engine/Graphics/Program.h>
#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include <Shared/Imgui/Context.h>
#include <Shared/Imgui/StateWindow.h>

#include "States/StupidState.h"
#include "UI/UIMainScreen.h"
#include "Main.h"

using namespace CubeWorld;
using namespace Game;

const double FRAMES_PER_SEC = 60.0;
const double SEC_PER_FRAME = (1 / FRAMES_PER_SEC);

int main(int argc, char **argv)
{
   using namespace Engine;

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

   Window::Options windowOptions;
   windowOptions.title = "Not Cube World";
   windowOptions.fullscreen = false;
   windowOptions.width = 1980;
   windowOptions.height = 1080;
   windowOptions.r = 14.f / 255.f;
   windowOptions.g = 180.f / 255.f;
   windowOptions.b = 204.f / 255.f;
   Window& window = Window::Instance();
   if (auto result = window.Initialize(windowOptions); !result)
   {
      LOG_ERROR("Failed creating window: %s", result.Failure().GetMessage());
      return 1;
   }

   std::unique_ptr<Engine::State> initialState = std::make_unique<StupidState>(window);
   Engine::StateManager& stateManager = Engine::StateManager::Instance();

   stateManager.SetState(std::move(initialState));

   // Set up imgui
   Editor::ImguiContext imgui(window);

   DebugHelper& debug = DebugHelper::Instance();
   debug.SetBounds(&window);

   std::unique_ptr<UIMainScreen> ui = std::make_unique<UIMainScreen>(&window);

   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug.RegisterMetric("FPS", [&clock]() -> std::string {
      return FormatString("%.1f", std::round(1.0 / clock.Average()));
   });

   // Setup input
   auto _ = window.AddCallback(GLFW_KEY_ESCAPE, [&](int, int, int) {
      window.SetShouldClose(true);
   });

   double timemod = 1.0;
   auto _2 = window.AddCallback(GLFW_KEY_O, [&](int, int, int) {
      timemod = 11.0 - timemod;
   });

   bool pause = false;
   auto _3 = window.AddCallback(GLFW_KEY_P, [&](int, int, int) {
      pause = !pause;

      window.SetMouseLock(!pause);
   });

   bool advance = false;
   auto _4 = window.AddCallback(GLFW_KEY_I, [&](int, int, int) {
      advance = true;
   });

   do {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         window.Clear();
         window.Update();

         double dtActual = std::min(elapsed, SEC_PER_FRAME);
         double dt = (pause && !advance) ? 0 : dtActual / timemod;

         imgui.StartFrame(dtActual);

         stateManager.Update(dt);
         ui->Update(dt);
         advance = false;

         GLenum error = glGetError();
         assert(error == 0);

         debug.Update(true);
         debug.Render(true);

         imgui.Render();

         error = glGetError();
         assert(error == 0);

         // Swap buffers
         window.SwapBuffers();
         glfwPollEvents();
      }
   } // Check if the ESC key was pressed or the window was closed
   while (!window.ShouldClose());

   stateManager.Shutdown();

   return 0;
}
