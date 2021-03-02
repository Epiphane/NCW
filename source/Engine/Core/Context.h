// By Thomas Steinke

#pragma once

#include <mutex>

#include "Window.h"

namespace CubeWorld
{

namespace Engine
{

// -------------------------- IMPORTANT NOTE ---------------------------
//              (see VBO.h for a more thorough explanation)
//
// Right now, multi-window isn't supported, so to make that explicit
// the Window class is a Singleton. One day, it would be nice to enable
// multi-window support, but that would require lots of change and it's
// not really worth pretending it's possible right now.

// Wrapper for a graphics context, that can share and create resources for a Window
class Context : public Singleton<Context>
{
public:
    // Instantiate this by calling Window::Instance().Initialize(...);
    Context();
    ~Context();

    void Initialize(Window& shared);

    void Activate();
    void Deactivate();

private:
    GLFWwindow* mGLFW;
    Graphics::VAO mVAO;

    std::mutex mActiveMutex;
    bool mActive = false;
};

}; // namespace Engine

}; // namespace CubeWorld
