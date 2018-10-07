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
#include <Engine/Core/Timer.h>
#include <Engine/Core/Window.h>

#include <Shared/Helpers/Asset.h>
#include <Shared/DebugHelper.h>

#include "AnimationStation/Editor.h"
#include "Command/CommandStack.h"
#include "Command/Commands.h"
#include "UI/Controls.h"
#include "UI/UISwapper.h"

#include "Main.h"

using namespace CubeWorld;
using namespace Engine;

const double FRAMES_PER_SEC = 60.0;
const double SEC_PER_FRAME = (1 / FRAMES_PER_SEC);

int main(int argc, char** argv)
{
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

   // Swaps between the different editors
   Editor::UISwapper windowContent;

   // Create subwindow for each editor
   Editor::AnimationStation::Editor* animationStation = windowContent.Add<Editor::AnimationStation::Editor>(*window);

   // Create the overarching Editor controls.
   /*Editor::Controls* controls = Editor::Controls>();

   // Always enable controls.
   // TODO this relies on SubWindowSwapper "forgetting" about this element, so swapping to
   // it later on would disable the element. There's probably a better way (e.g. creating
   // another base SubWindow, but it seems overkill /shrug
   controls->SetLayout({{
      Editor::Controls::Layout::Element{"Model Maker", [&](){
         // Editor::CommandStack::Instance()->Do<Editor::NavigateCommand>(&windowContent, modelMaker);
         // modelMaker->Start();
      }},
      Editor::Controls::Layout::Element{"Animation Station", [&](){
         Editor::CommandStack::Instance()->Do<Editor::NavigateCommand>(&windowContent, animationStation);
         animationStation->Start();
      }},
      Editor::Controls::Layout::Element{"Quit", [&]() {
         window->SetShouldClose(true);
      }}
   }});*/

   // Configure Debug helper
   Game::DebugHelper* debug = Game::DebugHelper::Instance();

   // FPS clock
   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug->RegisterMetric("FPS", [&clock]() -> std::string {
      return Format::FormatString("%.1f", std::round(1.0 / clock.Average()));
   });

   // Attach mouse events to state
   window->GetInput()->OnMouseDown([&](int button, double x, double y) {
      windowContent.GetCurrent()->Emit<MouseDownEvent>(button, x, y);
   });
   window->GetInput()->OnMouseUp([&](int button, double x, double y) {
      windowContent.GetCurrent()->Emit<MouseUpEvent>(button, x, y);
   });
   window->GetInput()->OnClick([&](int button, double x, double y) {
      windowContent.GetCurrent()->Emit<MouseClickEvent>(button, x, y);
   });

   // Save the pointers so that the callback doesn't get deregistered.
   auto _1 = window->GetInput()->AddCallback(Engine::Input::CtrlKey(GLFW_KEY_Z), [&](int, int, int) {
      Editor::CommandStack::Instance()->Undo();
   });
   auto _2 = window->GetInput()->AddCallback({
      Engine::Input::CtrlShiftKey(GLFW_KEY_Z),
      Engine::Input::CtrlKey(GLFW_KEY_Y)
   }, [&](int, int, int) {
      Editor::CommandStack::Instance()->Redo();
   });

   // Start in Animation Station
   animationStation->Start();
   windowContent.Swap(animationStation);

   Timer<100> windowContentRender;
   auto _3 = debug->RegisterMetric("Editor Render time", [&windowContentRender]() -> std::string {
      return Format::FormatString("%.1f", windowContentRender.Average());
   });

   windowContent.GetCurrent()->UpdateRoot();

   do {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         TIMEDELTA dt = std::min(elapsed, SEC_PER_FRAME);

         // Basic prep
         window->Clear();
         window->GetInput()->Update();

         // Render game state
         {
            windowContentRender.Reset();
            // TODO call UpdateRoot to resolve everything?
            windowContent.GetCurrent()->Update(dt);
            windowContent.GetCurrent()->RenderRoot();
            windowContentRender.Elapsed();
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

   // We don't use the StateManager, so don't shut it down.

   return 0;
}
