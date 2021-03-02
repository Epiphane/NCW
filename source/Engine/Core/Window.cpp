// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include "Window.h"
#include "Context.h"

namespace CubeWorld
{

namespace Engine
{

//
// Implemented below with the rest of the window input functions.
//
void keyCallback(GLFWwindow* glfw, int key, int scancode, int action, int mods);
void charCallback(GLFWwindow* glfw, unsigned int c);
void scrollCallback(GLFWwindow* glfw, double xoffset, double yoffset);
void mouseButtonCallback(GLFWwindow* glfw, int button, int action, int mods);

Window::Window() : mGLFW(nullptr)
{
   Reset();
}

Window::~Window()
{
   if (mGLFW != nullptr)
   {
      glfwDestroyWindow(mGLFW);
   }

   glfwTerminate();
}

Maybe<Window*> Window::Initialize(const Options& options)
{
   mOptions = options;
   if (!glfwInit()) {
      return Failure{"Failed to initialize GLFW"};
   }

   // Get monitor dimensions.
   const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   int monitorWidth = mode->width;
   int monitorHeight = mode->height;

   // Use the monitor size as a default
   if (mOptions.width < 0 || mOptions.height < 0)
   {
      mOptions.width = monitorWidth;
      mOptions.height = monitorHeight;
   }

   // Window settings. We don't wanna change these.
   glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   // Create the window itself
   GLFWmonitor *monitor = mOptions.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
   GLFWwindow *newWindow = glfwCreateWindow(mOptions.width, mOptions.height, mOptions.title.c_str(), monitor, mGLFW);
   if (newWindow == nullptr) {
      return Failure{"Failed to initialize GLFW"};
   }

   // Clean up any old window that might have existed.
   if (mGLFW != nullptr) {
      glfwDestroyWindow(mGLFW);
   }
   
   // Resize and reposition!
   mGLFW = newWindow;
   glfwGetWindowSize(mGLFW, &mOptions.width, &mOptions.height);
   glfwSetWindowPos(mGLFW, mOptions.x + monitorWidth / 2 - mOptions.width / 2, mOptions.y + monitorHeight / 2 - mOptions.height / 2);
   glfwMakeContextCurrent(mGLFW);

   // Initialize OpenGL context
   if (!gladLoadGL())
   {
      return Failure{"Failed to initialize glad!"};
   }

   // Clear any errors that might be lying around.
   glGetError();

   // Configure GL
   {
      //glEnable(GL_MULTISAMPLE);
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_BLEND);
      glClearColor(mOptions.r, mOptions.g, mOptions.b, mOptions.a);
   }

   // Get some info about the situation
   {
      const GLubyte *vendor = glGetString(GL_VENDOR);
      const GLubyte *renderer = glGetString(GL_RENDERER);
      LOG_INFO("Vendor: %s", vendor);
      LOG_INFO("Renderer: %s", renderer);
   }

   mVAO.Bind();

   Context::Instance().Initialize(*this);

   // Initialize input.
   Clear();

   glfwSetWindowUserPointer(mGLFW, this);

   // Ensure we can capture the escape key being pressed
   glfwSetInputMode(mGLFW, GLFW_STICKY_KEYS, GL_TRUE);

   glfwSetKeyCallback(mGLFW, &keyCallback);
   glfwSetCharCallback(mGLFW, &charCallback);
   glfwSetScrollCallback(mGLFW, &scrollCallback);
   glfwSetMouseButtonCallback(mGLFW, &mouseButtonCallback);
   SetMouseLock(mOptions.lockCursor);

   return this;
}

void Window::Clear()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::SwapBuffers()
{
   glfwSwapBuffers(mGLFW);
}

void Window::SetShouldClose(bool close)
{
   glfwSetWindowShouldClose(mGLFW, close);
}

bool Window::ShouldClose()
{
   return glfwWindowShouldClose(mGLFW) != 0;
}

void Window::Focus()
{
   glfwFocusWindow(mGLFW);
}

//
// Input Functions
//
void keyCallback(GLFWwindow* glfw, int key, int /*scancode*/, int action, int mods)
{
   Window *window = (Window *)glfwGetWindowUserPointer(glfw);
   // TODO validate this is a real Window

   window->TriggerKeyCallbacks(key, action, mods);
}

void charCallback(GLFWwindow* glfw, unsigned int c)
{
   Window* window = (Window*)glfwGetWindowUserPointer(glfw);
   // TODO validate this is a real Window

   window->TriggerCharCallbacks(c);
}

void scrollCallback(GLFWwindow* glfw, double xoffset, double yoffset)
{
   Window *window = (Window *)glfwGetWindowUserPointer(glfw);
   // TODO validate this is a real window

   window->mMouseScroll[0] += xoffset;
   window->mMouseScroll[1] += yoffset;
}

void mouseButtonCallback(GLFWwindow* glfw, int button, int action, int /*mods*/)
{
   Window *window = (Window *)glfwGetWindowUserPointer(glfw);
   // TODO validate this is a real Window

   double w_width = static_cast<double>(window->GetWidth());
   double w_height = static_cast<double>(window->GetHeight());

   double xpos, ypos;
   glfwGetCursorPos(glfw, &xpos, &ypos);
   // Fix y-axis
   ypos = w_height - ypos;
   window->mMousePressed[button] = (action == GLFW_PRESS);

   // If the mouse is pressed, then try for a MouseDown callback.
   if (window->mMousePressed[button])
   {
      if (window->mMouseDownCallback)
      {
         window->mMouseDownCallback(button, xpos / w_width, ypos / w_height);
      }
      window->mMousePressOrigin[button] = {xpos, ypos};
   }
   // Otherwise, it's released - always issue MouseUp, but only call MouseClick
   // if we didn't move far.
   else
   {
      if (window->mMouseUpCallback)
      {
         window->mMouseUpCallback(button, xpos / w_width, ypos / w_height);
      }

      if (!window->mMouseDragging[button])
      {
         if (window->mMouseClickCallback)
         {
            window->mMouseClickCallback(button, xpos / w_width, ypos / w_height);
         }
      }
      window->mMouseDragging[button] = false;
   }
}

void Window::SetMouseLock(bool locked)
{
   mMouseLocked = locked;

   if (locked) {
      glfwSetInputMode(mGLFW, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   }
   else {
      glfwSetInputMode(mGLFW, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   }
}

void Window::Reset()
{
   Input::Reset();
   mMouseLocked = false;
   mMousePosition = {0, 0};
   mMouseMovement = {0, 0};
   mLastMouseScroll = {0, 0};
   mMouseScroll = {0, 0};
   memset(mMousePressed, 0, sizeof(mMousePressed));
   memset(mMouseDragging, 0, sizeof(mMouseDragging));
   memset(mMousePressOrigin, 0, sizeof(mMousePressOrigin));
}

void Window::Update()
{
   glm::tvec2<double> mousePosition;

   // Don't lock up mouse
   glfwGetCursorPos(mGLFW, &mousePosition.x, &mousePosition.y);
   // Fix y-axis
   mousePosition.y = GetHeight() - mousePosition.y;
   mMouseMovement = mousePosition - mMousePosition;

   mLastMouseScroll = mMouseScroll;
   mMouseScroll = {0, 0};

   if (!mMouseLocked) {
      for (int button = GLFW_MOUSE_BUTTON_1; button < GLFW_MOUSE_BUTTON_LAST; ++button)
      {
         if (mMousePressed[button] && !mMouseDragging[button])
         {
            glm::tvec2<double> diff = mousePosition - mMousePressOrigin[button];
            double dist = diff.x * diff.x + diff.y * diff.y;
            if (dist > 2)
            {
               mMouseDragging[button] = true;
            }
         }
      }
   }
   else
   {
      // Edge case: window initialization makes things funky
      if (std::abs(mMouseMovement.x) > 200 ||
          std::abs(mMouseMovement.y) > 200 ||
          mousePosition == glm::tvec2<double>(0)) {
         mMouseMovement = {0, 0};
      }
   }

   mMousePosition = mousePosition;

   if (mMouseLocked) {
      glm::tvec2<double> middle = glm::tvec2<double>(GetWidth(), GetHeight()) / 2.0;

      glfwSetCursorPos(mGLFW, middle.x, middle.y);
      mMousePosition = middle;
   }
}

bool Window::IsKeyDown(int key) const
{
   return glfwGetKey(mGLFW, key) == GLFW_PRESS;
}

glm::tvec2<double> Window::GetRawMousePosition() const
{
   return mMousePosition;
}

glm::tvec2<double> Window::GetMousePosition() const
{
   return mMousePosition / glm::tvec2<double>(GetWidth(), GetHeight());
}

glm::tvec2<double> Window::CorrectYCoordinate(glm::tvec2<double> position) const
{
   return glm::tvec2<double>{position.x, GetHeight() - position.y};
}

}; // namespace Engine

}; // namespace CubeWorld
