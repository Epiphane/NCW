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
   auto _ = window->GetInput()->AddCallback(GLFW_KEY_ESCAPE, [&](int,int,int){
      window->SetShouldClose(true);
   });

   // Swaps between the different editors
   UI::Swapper windowContent;

   // Create subwindow for each editor
   Editor::AnimationStation::Editor* animationStation = windowContent.Add<Editor::AnimationStation::Editor>(*window);
   animationStation->AddConstraints({animationStation->GetFrame().z >= 10.0});

   Editor::ModelMaker::Editor* modelMaker = windowContent.Add<Editor::ModelMaker::Editor>(*window);
   modelMaker->AddConstraints({modelMaker->GetFrame().z >= -0.5});
   
   Editor::Constrainer::Editor* constrainer = windowContent.Add<Editor::Constrainer::Editor>(*window);
   constrainer->AddConstraints({constrainer->GetFrame().z >= 11.0});
    
   // Create editor-wide controls pane
   UIRoot controls(*window);
   {
      RectFilled* bg = controls.Add<RectFilled>(glm::vec4(0.2, 0.2, 0.2, 1));
      RectFilled* fg = controls.Add<RectFilled>(glm::vec4(0, 0, 0, 1));

      TextButton::Options buttonOptions;
      buttonOptions.text = "Animation Station";
      buttonOptions.onClick = [&]() {
         Editor::CommandStack::Instance()->Do<Editor::NavigateCommand>(&windowContent, animationStation);
         animationStation->Start();
      };
      UIFrame& fAnimationStation = controls.Add<TextButton>(buttonOptions)->GetFrame();

      buttonOptions.text = "Model Maker";
      buttonOptions.onClick = [&]() {
         Editor::CommandStack::Instance()->Do<Editor::NavigateCommand>(&windowContent, modelMaker);
         modelMaker->Start();
      };
      TextButton* modelMakerButton = controls.Add<TextButton>(buttonOptions);
      UIFrame& fModelMaker = modelMakerButton->GetFrame();
      
      buttonOptions.text = "Constrainer";
      buttonOptions.onClick = [&]() {
         Editor::CommandStack::Instance()->Do<Editor::NavigateCommand>(&windowContent, constrainer);
         constrainer->Start();
      };
      TextButton* constrainerButton = controls.Add<TextButton>(buttonOptions);

      buttonOptions.text = "Quit";
      buttonOptions.onClick = [&]() {
         window->SetShouldClose(true);
      };
      TextButton* quitButton = controls.Add<TextButton>(buttonOptions);
      UIFrame& fQuit = quitButton->GetFrame();
      
      constrainerButton->ConstrainBelow(modelMakerButton, 8.0);
      constrainerButton->ConstrainAbove(quitButton, 8.0);
      constrainerButton->ConstrainWidthTo(modelMakerButton);
      constrainerButton->ConstrainHeight(32);
      constrainerButton->ConstrainLeftAlignedTo(modelMakerButton);

      UIFrame& fControls = controls.GetFrame();
      Engine::UIFrame& fBackground = bg->GetFrame();
      Engine::UIFrame& fForeground = fg->GetFrame();
      controls.AddConstraints({
         fControls > fForeground,
         fForeground > fBackground,
         fBackground.z <= 10.0,

         fBackground.left == fControls.left,
         fBackground.right == window->GetWidth() * 0.2,
         fBackground.top == fAnimationStation.top + 8,
         fBackground.bottom == fControls.bottom,

         fForeground.left == fBackground.left + 2,
         fForeground.right == fBackground.right - 2,
         fForeground.top == fBackground.top - 2,
         fForeground.bottom == fBackground.bottom + 2,

         fAnimationStation.left == fBackground.left + 8,
         fAnimationStation.right == fBackground.right - 8,
         fAnimationStation.bottom == fModelMaker.top + 8,
         fAnimationStation.height == 32,

         fModelMaker.left == fAnimationStation.left,
         fModelMaker.right == fAnimationStation.right,
         fModelMaker.height == 32,

         fQuit.left == fModelMaker.left,
         fQuit.right == fModelMaker.right,
         fQuit.bottom == fControls.bottom + 16,
         fQuit.height == 32,
      });

      controls.UpdateRoot();
   }

   // Configure Debug helper
   DebugHelper* debug = DebugHelper::Instance();

   // FPS clock
   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug->RegisterMetric("FPS", [&clock]() -> std::string {
      return Format::FormatString("%.1f", std::round(1.0 / clock.Average()));
   });

   // Attach mouse events to state
   window->GetInput()->OnMouseDown([&](int button, double x, double y) {
      x *= window->GetWidth();
      y *= window->GetHeight();
      windowContent.GetCurrent()->Emit<MouseDownEvent>(button, x, y);
      controls.Emit<MouseDownEvent>(button, x, y);
   });
   window->GetInput()->OnMouseUp([&](int button, double x, double y) {
      x *= window->GetWidth();
      y *= window->GetHeight();
      windowContent.GetCurrent()->Emit<MouseUpEvent>(button, x, y);
      controls.Emit<MouseUpEvent>(button, x, y);
   });
   window->GetInput()->OnClick([&](int button, double x, double y) {
      x *= window->GetWidth();
      y *= window->GetHeight();
      windowContent.GetCurrent()->Emit<MouseClickEvent>(button, x, y);
      controls.Emit<MouseClickEvent>(button, x, y);
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

   // Start in Model Maker
//   modelMaker->Start();
   animationStation->Start();
//   constrainer->Start();
   windowContent.Swap(animationStation);

   Timer<100> windowContentRender;
   auto _3 = debug->RegisterMetric("Editor Render time", [&windowContentRender]() -> std::string {
      return Format::FormatString("%.1f", windowContentRender.Average());
   });

   Timer<100> uiUpdateTime;
   auto _4 = debug->RegisterMetric("UI UpdateRoot", [&uiUpdateTime]() -> std::string {
      return Format::FormatString("%.1f", uiUpdateTime.Average());
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

         glm::tvec2<double> pos = window->GetInput()->GetRawMousePosition();
         windowContent.GetCurrent()->Emit<MouseMoveEvent>(pos.x, pos.y);
         controls.Emit<MouseMoveEvent>(pos.x, pos.y);

         // Render game state
         {
            windowContentRender.Reset();
            uiUpdateTime.Reset();
            windowContent.GetCurrent()->UpdateRoot();
            uiUpdateTime.Elapsed();
            windowContent.GetCurrent()->Update(dt);
            windowContent.GetCurrent()->RenderRoot();

            controls.Update(dt);
            controls.RenderRoot();
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
