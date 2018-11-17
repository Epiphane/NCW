//
// NotCubeWorld - An attempt to make something that looks like CubeWorld
//

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <GL/includes.h>
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

#include "States/StupidState.h"
#include "UI/UIMainScreen.h"
#include "Main.h"

using namespace CubeWorld;
using namespace Game;

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
   Window* window = Window::Instance();
   if (auto result = window->Initialize(windowOptions); !result)
   {
      LOG_ERROR("Failed creating window: %s", result.Failure().GetMessage());
      return 1;
   }

   std::unique_ptr<Engine::State> initialState = std::make_unique<StupidState>(window);
   Engine::StateManager* stateManager = Engine::StateManager::Instance();

   stateManager->SetState(std::move(initialState));

   DebugHelper* debug = DebugHelper::Instance();
   debug->SetBounds(window);
   
   std::unique_ptr<UIMainScreen> ui = std::make_unique<UIMainScreen>();
   ui->SetBounds(*window);

   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug->RegisterMetric("FPS", [&clock]() -> std::string {
      return Format::FormatString("%.1f", std::round(1.0 / clock.Average()));
   });

   // Setup input
   auto _ = window->GetInput()->AddCallback(GLFW_KEY_ESCAPE, [&](int, int, int) {
      window->SetShouldClose(true);
   });

   do {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         window->Clear();
         window->GetInput()->Update();

         stateManager->Update(std::min(elapsed, SEC_PER_FRAME));

         GLenum error = glGetError();
         assert(error == 0);

         debug->Update();
         debug->Render();
         
         ui->UpdateRoot();
         ui->RenderRoot();

         error = glGetError();
         assert(error == 0);

         // Swap buffers
         window->SwapBuffers();
         glfwPollEvents();
      }
   } // Check if the ESC key was pressed or the window was closed
   while (!window->ShouldClose());

   stateManager->Shutdown();

   return 0;
}
