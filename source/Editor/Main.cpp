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

#include "AnimationStation/Editor.h"
#include "Command/CommandStack.h"
#include "Command/Commands.h"
#include "UI/Controls.h"
#include "UI/StateWindow.h"
#include "UI/SubWindowSwapper.h"

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
   windowOptions.lockCursor = false;
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
   Editor::SubWindowSwapper::Options windowContentOptions;
   Editor::SubWindowSwapper windowContent(*window, windowContentOptions);

   // Create subwindow for the current editor
   Editor::AnimationStation::Editor::Options animationStationOptions;
   Editor::AnimationStation::Editor* animationStation = windowContent.Add<Editor::AnimationStation::Editor>(animationStationOptions);

   // Create subwindow for the overarching Editor controls.
   Editor::Controls::Options controlsOptions;
   controlsOptions.w = 0.2f;
   controlsOptions.h = 0.2f;
   Editor::Controls* controls = windowContent.Add<Editor::Controls>(controlsOptions);

   // Always enable controls.
   // TODO this relies on SubWindowSwapper "forgetting" about this element, so swapping to
   // it later on would disable the element. There's probably a better way (e.g. creating
   // another base SubWindow, but it seems overkill /shrug
   controls->SetActive(true);
   controls->SetLayout({{
      Editor::Controls::Layout::Element{"Animation Station", [&](){
         Editor::CommandStack::Instance()->Do<Editor::NavigateCommand>(&windowContent, animationStation);
      }},
      Editor::Controls::Layout::Element{"Quit", [&]() {
         window->SetShouldClose(true);
      }}
   }});

   // Configure Debug helper
   Game::DebugHelper* debug = Game::DebugHelper::Instance();

   // FPS clock
   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug->RegisterMetric("FPS", [&clock]() -> std::string {
      return Format::FormatString("%.1f", std::round(1.0 / clock.Average()));
   });

   // Start with AnimationStation
   animationStation->Start();

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
   // Save the pointers so that the callback doesn't get deregistered.
   auto _1 = window->GetInput()->AddCallback(Engine::Input::CtrlKey(GLFW_KEY_Z), [&](int, int, int) {
      Editor::CommandStack::Instance()->Undo();
   });
   auto _2 = window->GetInput()->AddCallback(Engine::Input::CtrlKey(GLFW_KEY_Y), [&](int, int, int) {
      Editor::CommandStack::Instance()->Redo();
   });

   // Start in Animation Station
   windowContent.Swap(animationStation);

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
         }

         // Render debug stuff
         {
            debug->Update();
            debug->Render();
         }

         // Swap buffers
         {
            window->SwapBuffers();
            glfwPollEvents();
         }
      }
   } // Check if the ESC key was pressed or the window was closed
   while (!window->ShouldClose());

   Engine::StateManager::Instance()->Shutdown();

   return 0;
}
