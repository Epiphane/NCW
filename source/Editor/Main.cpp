//
// NotCubeWorld - An attempt to make something that looks like CubeWorld
//

#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <cmath>

#include <Engine/Logger/Logger.h>
#include <Engine/Logger/StdoutLogger.h>
#include <Engine/Logger/DebugLogger.h>
#include <Engine/Core/Input.h>
#include <Engine/Core/StateManager.h>
#include <Engine/Core/Timer.h>
#include <Engine/Core/Window.h>

#include <Shared/DebugHelper.h>

#include "Helpers/Controls.h"
#include "Helpers/SubWindow.h"
#include "States/AnimationStation.h"

#include "Main.h"

using namespace CubeWorld;
using namespace Engine;

const double FRAMES_PER_SEC = 60.0;
const double SEC_PER_FRAME = (1 / FRAMES_PER_SEC);

// TODO a better UI/constraints system.
// There will be a REAL one in the game itself, but
// maybe we can make it work for the simple 3-panel
// setup that the Editor's got. Who knows. ¯\_(ツ)_/¯
const float SIDEBAR_X = 0.0f;
const float SIDEBAR_Y = 0.0f;
const float SIDEBAR_W = 0.2f;
const float SIDEBAR_H = 1.0f;
const float DOCK_X = SIDEBAR_W;
const float DOCK_Y = 0.0f;
const float DOCK_W = 0.8f;
const float DOCK_H = 0.2f;
const float MAIN_X = SIDEBAR_W;
const float MAIN_Y = DOCK_H;
const float MAIN_W = 1.0f - SIDEBAR_W;
const float MAIN_H = 1.0f - DOCK_H;

int main(int /* argc */, char ** /* argv */) {
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
   Input::InputManager::Initialize(window);
   Input::InputManager* input = Input::InputManager::Instance();
   input->Clear();
   input->SetCallback(GLFW_KEY_ESCAPE, [&]{
      glfwSetWindowShouldClose(window->get(), GL_TRUE);
   });

   // Create subwindow for the controls
   Editor::SubWindow::Options controlsWindowOptions;
   controlsWindowOptions.x = SIDEBAR_X * window->Width();
   controlsWindowOptions.y = SIDEBAR_Y * window->Height();
   controlsWindowOptions.w = SIDEBAR_W * window->Width();
   controlsWindowOptions.h = SIDEBAR_H * window->Height();
   Editor::SubWindow controlsWindow(window, controlsWindowOptions);

   // Create controls sidebar
   std::unique_ptr<Editor::Controls> controls = std::make_unique<Editor::Controls>(&controlsWindow);

   // Create subwindow for the state
   Editor::SubWindow::Options gameWindowOptions;
   gameWindowOptions.x = MAIN_X * window->Width();
   gameWindowOptions.y = MAIN_Y * window->Height();
   gameWindowOptions.w = MAIN_W * window->Width();
   gameWindowOptions.h = MAIN_H * window->Height();
   Editor::SubWindow gameWindow(window, gameWindowOptions);

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
   stateManager->SetState(std::make_unique<Editor::AnimationStation>(float(gameWindowOptions.w) / gameWindowOptions.h, controls.get()));

   // Attach mouse events to state
   // TODO this is hella hacky, don't tell on me...
#define MOUSE_EVENT(Type) [&](int button, double x, double y) {\
      if (x < MAIN_X)\
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
         x = (x - MAIN_X) / MAIN_W;\
         y = (y - MAIN_Y) / MAIN_H;\
         stateManager->Emit<Type##Event>(button, x, y);\
      }\
   }

   input->OnMouseDown(MOUSE_EVENT(MouseDown));
   input->OnMouseUp(MOUSE_EVENT(MouseUp));
   input->OnDrag(MOUSE_EVENT(MouseDrag));
   input->OnClick(MOUSE_EVENT(MouseClick));

   do {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         GLenum error;

         // Basic prep
         window->Clear();
         input->Update();

         gameWindow.Bind();

         // Render game state
         {
            stateManager->Update(std::min(elapsed, SEC_PER_FRAME));

            error = glGetError();
            assert(error == 0);
         }

         // Render debug stuff
         {
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
