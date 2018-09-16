//
// NotCubeWorld - An attempt to make something that looks like CubeWorld
//

#include <algorithm>
#include <GL/includes.h>
#include <cassert>
#include <cmath>

#include <Engine/Logger/Logger.h>
#include <Engine/Logger/StdoutLogger.h>
#include <Engine/Logger/DebugLogger.h>
#include <Engine/Core/Input.h>
#include <Engine/Core/StateManager.h>
#include <Engine/Core/Timer.h>
#include <Engine/Core/Window.h>

#include <Shared/Helpers/Asset.h>
#include <Shared/DebugHelper.h>

#include "Helpers/Controls.h"
#include "UI/StateWindow.h"
#include "UI/SubWindow.h"
#include "AnimationStation/State.h"

#include "Main.h"

using namespace CubeWorld;
using namespace Engine;

const double FRAMES_PER_SEC = 60.0;
const double SEC_PER_FRAME = (1 / FRAMES_PER_SEC);

int main(int argc, char** argv)
{
   // Parse arguments
   argc;
   argv;

   // Initialize and register loggers to VS debugger and stdout
   Logger::StdoutLogger::Instance();
   Logger::DebugLogger::Instance();

   // Setup main window
   Window::Options windowOptions;
   windowOptions.title = "NCW Editor";
   windowOptions.fullscreen = false;
   windowOptions.width = 1280;
   windowOptions.height = 760;
   windowOptions.b = 0.4f;
   Window* window = Window::Instance();
   if (auto result = window->Initialize(windowOptions); !result)
   {
      LOG_ERROR("Failed creating window: %s", result.Failure().GetMessage());
      return 1;
   }
   
   // Setup input
   auto _ = window->GetInput()->AddCallback(GLFW_KEY_ESCAPE, [&](int,int,int){
      window->SetShouldClose(true);
   });

   // Create "SubWindow" that has everything in it
   Editor::SubWindow::Options windowContentOptions;
   windowContentOptions.x = 0;
   windowContentOptions.y = 0;
   windowContentOptions.w = 1;
   windowContentOptions.h = 1;
   Editor::SubWindow windowContent(*window, windowContentOptions);

   // Create subwindow for the controls
   Editor::Controls::Options controlsOptions;
   controlsOptions.x = 0.0f;
   controlsOptions.y = 0.0f;
   controlsOptions.w = 0.2f;
   controlsOptions.h = 1.0f;
   Editor::Controls* controls = windowContent.Add<Editor::Controls>(controlsOptions);

   // Create subwindow for the game state
   Editor::StateWindow::Options gameWindowOptions;
   gameWindowOptions.x = controlsOptions.w;
   gameWindowOptions.y = 0.3f;
   gameWindowOptions.w = 1.0f - controlsOptions.w;
   gameWindowOptions.h = 0.7f;
   Editor::StateWindow* gameWindow = windowContent.Add<Editor::StateWindow>(gameWindowOptions);

   // Configure Debug helper
   Game::DebugHelper* debug = Game::DebugHelper::Instance();
   debug->SetBounds(gameWindow);

   // FPS clock
   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug->RegisterMetric("FPS", [&clock]() -> std::string {
      return Format::FormatString("%1", std::round(1.0 / clock.Average()));
   });

   // Start with AnimationStation
   Engine::StateManager* stateManager = Engine::StateManager::Instance();
   stateManager->SetState(std::make_unique<Editor::AnimationStation::MainState>(window, *gameWindow, controls));

   // Attach mouse events to state
   window->GetInput()->OnMouseDown([&](int button, double x, double y) {
      windowContent.MouseDown(button, x, y);
   });
   window->GetInput()->OnMouseUp([&](int button, double x, double y) {
      windowContent.MouseUp(button, x, y);
   });
   window->GetInput()->OnDrag([&](int button, double x, double y) {
      windowContent.MouseDrag(button, x, y);
   });
   window->GetInput()->OnClick([&](int button, double x, double y) {
      windowContent.MouseClick(button, x, y);
   });

   do {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         TIMEDELTA dt = std::min(elapsed, SEC_PER_FRAME);

         // Basic prep
         window->Clear();
         window->GetInput()->Update();

         glm::tvec2<double> mouse = window->GetInput()->GetMousePosition();

         // Render game state
         {
            windowContent.MouseMove(mouse.x, mouse.y);
            windowContent.Update(dt);

            CHECK_GL_ERRORS();
         }

         // Render debug stuff
         {
            // TODO tech debt? It's getting covered by the stuff the same draws,
            // but I mean we always want debug text on top soooo can't hurt.
            glClear(GL_DEPTH_BUFFER_BIT);

            debug->Update();
            debug->Render();

            CHECK_GL_ERRORS();
         }

         // Swap buffers
         {
            window->SwapBuffers();
            glfwPollEvents();
         }
      }
   } // Check if the ESC key was pressed or the window was closed
   while (!window->ShouldClose());

   stateManager->Shutdown();

   return 0;
}
