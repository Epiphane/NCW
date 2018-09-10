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
#include "Helpers/SubWindow.h"
#include "AnimationStation/State.h"

#include "Main.h"

using namespace CubeWorld;
using namespace Engine;

const double FRAMES_PER_SEC = 60.0;
const double SEC_PER_FRAME = (1 / FRAMES_PER_SEC);

// TODO a better UI/constraints system.
// There will be a REAL one in the game itself, but
// maybe we can make it work for the simple 2-panel
// setup that the Editor's got. Who knows. ¯\_(ツ)_/¯
const float SIDEBAR_X = 0.0f;
const float SIDEBAR_Y = 0.0f;
const float SIDEBAR_W = 0.2f;
const float SIDEBAR_H = 1.0f;
const float MAIN_X = SIDEBAR_W;
const float MAIN_Y = 0.0f;
const float MAIN_W = 1.0f - SIDEBAR_W;
const float MAIN_H = 1.0f;

int main(int argc, char** argv)
{
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

   // Create subwindow for the controls
   Editor::SubWindow::Options controlsWindowOptions;
   controlsWindowOptions.x = SIDEBAR_X;
   controlsWindowOptions.y = SIDEBAR_Y;
   controlsWindowOptions.w = SIDEBAR_W;
   controlsWindowOptions.h = SIDEBAR_H;
   Editor::SubWindow controlsWindow(*window, controlsWindowOptions);

   // Create controls sidebar
   std::unique_ptr<Editor::Controls> controls = std::make_unique<Editor::Controls>(&controlsWindow);

   // Create subwindow for the state
   Editor::SubWindow::Options gameWindowOptions;
   gameWindowOptions.x = MAIN_X;
   gameWindowOptions.y = MAIN_Y;
   gameWindowOptions.w = MAIN_W;
   gameWindowOptions.h = MAIN_H;
   Editor::SubWindow gameWindow(*window, gameWindowOptions);

   // Configure Debug helper
   Game::DebugHelper* debug = Game::DebugHelper::Instance();
   debug->SetBounds(&gameWindow);

   // FPS clock
   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug->RegisterMetric("FPS", [&clock]() -> std::string {
      return Format::FormatString("%1", std::round(1.0 / clock.Average()));
   });

   // Start with AnimationStation
   Engine::StateManager* stateManager = Engine::StateManager::Instance();
   stateManager->SetState(std::make_unique<Editor::AnimationStation::MainState>(window, gameWindow, controls.get()));

   // Attach mouse events to state
   // TODO this is hella hacky, don't tell on me...
#define MOUSE_EVENT(Type) [&](int button, double x, double y) {\
      if (x < SIDEBAR_W)\
      {\
         x /= SIDEBAR_W;\
         y /= SIDEBAR_H;\
         controls->Type(button, x, y);\
      }\
      else if (y < MAIN_Y)\
      {\
         /* No dock yet */\
      }\
      else\
      {\
         x = (x - MAIN_X) / (MAIN_W);\
         y = (y - MAIN_Y) / (MAIN_H);\
         stateManager->Emit<Type##Event>(button, x, y);\
      }\
   }

   window->GetInput()->OnMouseDown(MOUSE_EVENT(MouseDown));
   window->GetInput()->OnMouseUp(MOUSE_EVENT(MouseUp));
   window->GetInput()->OnDrag(MOUSE_EVENT(MouseDrag));
   window->GetInput()->OnClick(MOUSE_EVENT(MouseClick));

   do {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         GLenum error;

         // Basic prep
         window->Clear();
         window->GetInput()->Update();

         gameWindow.Bind();

         // Render game state
         {
            stateManager->Update(std::min(elapsed, SEC_PER_FRAME));

            error = glGetError();
            assert(error == 0);
         }

         // Render debug stuff
         {
            // TODO tech debt? It's getting covered by the stuff the same draws,
            // but I mean we always want debug text on top soooo can't hurt.
            glClear(GL_DEPTH_BUFFER_BIT);

            debug->Update();
            debug->Render();

            error = glGetError();
            assert(error == 0);
         }

         // Pop game/debug into game section
         gameWindow.Unbind();
         gameWindow.Render();

         controlsWindow.Bind();

         // Render controls
         {
            glm::tvec2<double> position = window->GetInput()->GetMousePosition();
            if (position.x < SIDEBAR_W * window->Width())
            {
               controls->MouseMove(position.x / (SIDEBAR_W * window->Width()), position.y / (SIDEBAR_H * window->Height()));
            }
            else
            {
               controls->MouseMove(0, 0);
            }
            
            controls->Update();

            error = glGetError();
            assert(error == 0);
         }

         controlsWindow.Unbind();
         controlsWindow.Render();

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
