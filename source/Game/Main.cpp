//
// NotCubeWorld - An attempt to make something that looks like CubeWorld
//

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>
#include <stdio.h>

#include <Engine/Logger/Logger.h>
#include <Engine/Logger/StdoutLogger.h>
#include <Engine/Logger/DebugLogger.h>
#include <Engine/Core/Input.h>
#include <Engine/Core/StateManager.h>
#include <Engine/Core/Timer.h>
#include <Engine/Core/Window.h>
#include <Engine/Graphics/Program.h>

#include <Shared/DebugHelper.h>
#include <Shared/States/AnimationStation.h>

#include "States/StupidState.h"
#include "Main.h"

using namespace CubeWorld;

const double FRAMES_PER_SEC = 60.0;
const double SEC_PER_FRAME = (1 / FRAMES_PER_SEC);

void setWindowSizeDefault() {
   //const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   //setWindowSize(mode->width, mode->height);
}

int main(int /* argc */, char ** /* argv */) {
   using namespace Engine;

   // Initialize and register loggers to VS debugger and stdout
   Logger::StdoutLogger::Instance();
   Logger::DebugLogger::Instance();

   Window::Options windowOptions;
   windowOptions.title = "Not Cube World";
   windowOptions.fullscreen = false;
   windowOptions.width = 1280;
   windowOptions.height = 760;
   windowOptions.b = 0.4f;
   std::unique_ptr<Window> window = std::make_unique<Window>(windowOptions);
   
   Input::InputManager* input = Input::InputManager::Instance();
   input->Clear();

   std::unique_ptr<Engine::State> initialState = std::make_unique<Game::StupidState>(window.get());
   Engine::StateManager* stateManager = Engine::StateManager::Instance();

   stateManager->SetState(std::move(initialState));

   Game::DebugHelper* debug = Game::DebugHelper::Instance();
   debug->SetWindow(window.get());

   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug->RegisterMetric("FPS", [&clock]() -> std::string {
      return Format::FormatString("%1", std::round(1.0 / clock.Average()));
   });

   do {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         window->Clear();
         input->Update();

         stateManager->Update(std::min(elapsed, SEC_PER_FRAME));

         GLenum error = glGetError();
         assert(error == 0);

         debug->Update();
         debug->Render();

         error = glGetError();
         assert(error == 0);

         // Swap buffers
         window->SwapBuffers();
         glfwPollEvents();
      }
   } // Check if the ESC key was pressed or the window was closed
   while (!window->ShouldClose() && !input->IsKeyDown(GLFW_KEY_ESCAPE));

   stateManager->Shutdown();

   return 0;
}
