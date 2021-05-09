// By Thomas Steinke

#pragma once

#include <mutex>

#include "Window.h"

namespace CubeWorld
{

namespace Engine
{

// Wrapper for a graphics context, that can share and create resources for a Window
class Context
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

    //static std::vector<Context*> sContexts;
};

class ContextManager : public Singleton<ContextManager>
{
public:
    void Add(Context* ctx);
    void Remove(Context* ctx);
    std::vector<Context*> GetAll();

private:
    std::vector<Context*> mContexts;
};

}; // namespace Engine

}; // namespace CubeWorld
