#include <stdio.h>

#include <GL/includes.h>
#include <GLFW/glfw3native.h>

// #include <RGBNetworking/JSONSerializer.h>
#include <RGBLogger/DebugLogger.h>
#include <RGBLogger/StdoutLogger.h>
#include <Engine/Core/Window.h>

using namespace CubeWorld;
using namespace CubeWorld::Engine;

int main(int, char**)
{
   Logger::StdoutLogger::Instance();
   Logger::DebugLogger::Instance();

   // Setup main window
   Window::Options windowOptions;
   windowOptions.title = "NCW Editor";
   windowOptions.fullscreen = false;
   windowOptions.width = 1280;
   windowOptions.height = 760;
   windowOptions.lockCursor = false;
   Window& window = Window::Instance();
   if (auto result = window.Initialize(windowOptions); !result)
   {
      return 1;
   }

   auto _ = window.AddCallback(GLFW_KEY_ESCAPE, [&](int, int, int) {
      window.SetShouldClose(true);
   });

   do {
      // Swap buffers
      window.SwapBuffers();
      glfwPollEvents();
   } // Check if the ESC key was pressed or the window was closed
   while (!window.ShouldClose());

   return 0;
}
