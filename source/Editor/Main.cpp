//
// NotCubeWorld - An attempt to make something that looks like CubeWorld
//

#include <algorithm>
#include <GL/includes.h>
#include <cassert>
#include <cmath>

#include <Engine/Core/Input.h>
#include <Engine/Core/Timer.h>
#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Engine/Logger/StdoutLogger.h>
#include <Engine/Logger/DebugLogger.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include <Shared/UI/RectFilled.h>
#include <Shared/UI/Swapper.h>
#include <Shared/UI/TextButton.h>

#include "AnimationStation/Editor.h"
#include "Command/CommandStack.h"
#include "Command/Commands.h"
#include "ModelMaker/Editor.h"
#include "Constrainer/Editor.h"

#include "Controls.h"
#include "Main.h"

using namespace CubeWorld;
using namespace Engine;

using UI::RectFilled;
using UI::TextButton;

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
   windowOptions.lockCursor = false;
   Window* window = Window::Instance();
   if (auto result = window->Initialize(windowOptions); !result)
   {
      LOG_ERROR("Failed creating window: %s", result.Failure().GetMessage());
      return 1;
   }
   
   // Setup input
   auto _ = window->AddCallback(GLFW_KEY_ESCAPE, [&](int,int,int){
      window->SetShouldClose(true);
   });

   // Swaps between the different editors
   UI::Swapper windowContent;

   Editor::AnimationStation::Editor* animationStation = nullptr;
   Editor::ModelMaker::Editor* modelMaker = nullptr;
   Editor::Constrainer::Editor* constrainer = nullptr;
   Editor::Controls::Options controlsOptions{
      {
         "Animation Station",
         [&]() {
            Editor::CommandStack::Instance()->Do<Editor::NavigateCommand>(&windowContent, animationStation);
            animationStation->Start();
         }
      },
      {
         "Model Maker",
         [&]() {
            Editor::CommandStack::Instance()->Do<Editor::NavigateCommand>(&windowContent, modelMaker);
            modelMaker->Start();
         }
      },
      {
         "Constrainer",
         [&]() {
            Editor::CommandStack::Instance()->Do<Editor::NavigateCommand>(&windowContent, constrainer);
            constrainer->Start();
         }
      },
      {
         "Quit", [&]() { window->SetShouldClose(true); }
      }
   };

   // Create editors
   animationStation = windowContent.Add<Editor::AnimationStation::Editor>(window, controlsOptions);
   animationStation->SetBounds(*window);
   animationStation->SetName("Animation Station");

   modelMaker = windowContent.Add<Editor::ModelMaker::Editor>(window, controlsOptions);
   modelMaker->SetBounds(*window);
   modelMaker->SetName("Model Maker");

   constrainer = windowContent.Add<Editor::Constrainer::Editor>(window, controlsOptions);
   constrainer->SetBounds(*window);
   constrainer->SetName("Constrainer");

   // Configure Debug helper
   DebugHelper* debug = DebugHelper::Instance();

   // FPS clock
   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug->RegisterMetric("FPS", [&clock]() -> std::string {
      return Format::FormatString("%.1f", std::round(1.0 / clock.Average()));
   });

   // Attach mouse events to state
   window->OnMouseDown([&](int button, double x, double y) {
      x *= window->GetWidth();
      y *= window->GetHeight();
      windowContent.GetCurrent()->Emit<MouseDownEvent>(button, x, y);
   });
   window->OnMouseUp([&](int button, double x, double y) {
      x *= window->GetWidth();
      y *= window->GetHeight();
      windowContent.GetCurrent()->Emit<MouseUpEvent>(button, x, y);
   });
   window->OnClick([&](int button, double x, double y) {
      x *= window->GetWidth();
      y *= window->GetHeight();
      windowContent.GetCurrent()->Emit<MouseClickEvent>(button, x, y);
   });

   // Save the pointers so that the callback doesn't get deregistered.
   auto _1 = window->AddCallback(Engine::Window::CtrlKey(GLFW_KEY_Z), [&](int, int, int) {
      Editor::CommandStack::Instance()->Undo();
   });
   auto _2 = window->AddCallback({
      Engine::Window::CtrlShiftKey(GLFW_KEY_Z),
      Engine::Window::CtrlKey(GLFW_KEY_Y)
   }, [&](int, int, int) {
      Editor::CommandStack::Instance()->Redo();
   });

   // Start in Model Maker
   modelMaker->Start();
   windowContent.Swap(modelMaker);
   constrainer->Start();

   Timer<100> windowContentRender;
   auto _3 = debug->RegisterMetric("Editor Render time", [&windowContentRender]() -> std::string {
      return Format::FormatString("%.1f", windowContentRender.Average());
   });

   Timer<100> uiUpdateTime;
   auto _4 = debug->RegisterMetric("UI UpdateRoot", [&uiUpdateTime]() -> std::string {
      return Format::FormatString("%.1f", uiUpdateTime.Average());
   });

   uiUpdateTime.Reset();
   windowContent.GetCurrent()->UpdateRoot();
   LOG_ALWAYS("Time spent updating initial root: %1s", uiUpdateTime.Elapsed());

   do {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         TIMEDELTA dt = std::min(elapsed, SEC_PER_FRAME);

         // Basic prep
         window->Clear();
         window->Update();

         glm::tvec2<double> pos = window->GetRawMousePosition();
         windowContent.GetCurrent()->Emit<MouseMoveEvent>(pos.x, pos.y);

         // Render game state
         {
            windowContentRender.Reset();
            uiUpdateTime.Reset();
            windowContent.GetCurrent()->UpdateRoot();
            uiUpdateTime.Elapsed();
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
