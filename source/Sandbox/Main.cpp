#include <stdio.h>

#include <ShlObj.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GL/includes.h>
#include <GLFW/glfw3native.h>

#include <RGBNetworking/JSONSerializer.h>
#include <RGBLogger/DebugLogger.h>
#include <RGBLogger/StdoutLogger.h>
#include <Engine/Core/Window.h>

using namespace CubeWorld;
using namespace CubeWorld::Engine;

WNDPROC prevWndProc;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   char msss[128];
   sprintf(msss, "Message: %04x (%u) %x %x", message, message, wParam, lParam);

   LOG_INFO(msss);

   return CallWindowProc(prevWndProc, hWnd, message, wParam, lParam);
}

int main(int argc, char** argv)
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
   Window* window = Window::Instance();
   if (auto result = window->Initialize(windowOptions); !result)
   {
      return 1;
   }

   auto _ = window->AddCallback(GLFW_KEY_ESCAPE, [&](int, int, int) {
      window->SetShouldClose(true);
   });

   ITaskbarList3* ptl; 
   CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList3, (void**)&ptl);

   wchar_t val[2048];
   DWORD size;
   HKEY hkResult;
   LSTATUS stat = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Taskband", 0, KEY_READ, &hkResult);
   if (stat == ERROR_SUCCESS)
   {
      stat = RegQueryValueExW(hkResult, L"Favorites", nullptr, nullptr, (LPBYTE)val, &size);

      if (stat == ERROR_SUCCESS)
      {
         printf("Success\n");
      }
   }

   long progress = 0;

   HWND h = glfwGetWin32Window(window->get());

   prevWndProc = (WNDPROC) SetWindowLongPtr(h, GWL_WNDPROC, (LONG_PTR)&WndProc);

   do {
      //progress += 1;

      //ptl->SetProgressState(h, TBPF_NOPROGRESS);
      //ptl->SetProgressValue(h, progress, 100);

      //SendMessage

      // Swap buffers
      //window->SwapBuffers();
      Sleep(10);
      glfwPollEvents();
   } // Check if the ESC key was pressed or the window was closed
   while (!window->ShouldClose() && progress < 100);

   return 0;
}