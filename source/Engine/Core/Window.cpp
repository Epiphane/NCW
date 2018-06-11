// By Thomas Steinke

#include <GL/glew.h>

#include <Engine/Core/Input.h>
#include <Engine/Logger/Logger.h>
#include <Engine/Graphics/VAO.h>
#include "Window.h"

namespace CubeWorld
{

namespace Engine
{

Window::Window(Window::Options options)
   : window(nullptr)
   , mOptions(options)
{
   if (!glfwInit()) {
      LOG_ERROR("Failed to initialize GLFW");
      return;
   }

   if (options.width < 0 || options.height < 0)
   {
      const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      options.width = mode->width;
      options.height = mode->height;
   }

   glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   GLFWmonitor *monitor = options.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
   GLFWwindow *newWindow = glfwCreateWindow(options.width, options.height, options.title.c_str(), monitor, window);
   if (newWindow == nullptr) {
      LOG_ERROR("Failed to initialize GLFW");
      return;
   }

   if (window != nullptr) {
      glfwDestroyWindow(window);
   }
   window = newWindow;

   glfwMakeContextCurrent(window);
   Input::InputManager::Initialize(this);

   glewExperimental = true;

   if (GLenum result = glewInit(); result != GLEW_OK) {
      LOG_ERROR("Failed to initialize GLEW: %s", glewGetErrorString(result));
      return;
   }

   // Clear any errors that might be lying around.
   glGetError();

   //glEnable(GL_MULTISAMPLE);
   //glEnable(GL_BLEND);
   //glEnable(GL_DEPTH_TEST);
   //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glClearColor(options.r, options.g, options.b, options.a);

   mVAO.Bind();

   // Get some info about the situation
   const GLubyte *vendor = glGetString(GL_VENDOR);
   const GLubyte *renderer = glGetString(GL_RENDERER);
   LOG_INFO("Vendor: %s", vendor);
   LOG_INFO("Renderer: %s", renderer);

   if (options.lockCursor)
   {
      LockCursor();
   }
   {GLenum error = glGetError();
   assert(error == 0); }
}

Window::~Window()
{
   if (window != nullptr)
   {
      glfwDestroyWindow(window);
   }

   glfwTerminate();
}

void Window::LockCursor()
{
   glfwSetCursorPos(window, mOptions.width / 2, mOptions.height / 2);
}

void Window::UnlockCursor()
{
}

}; // namespace Engine

}; // namespace CubeWorld
