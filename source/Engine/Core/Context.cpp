// By Thomas Steinke

#include "Context.h"

namespace CubeWorld
{

namespace Engine
{

/// 
/// 
/// 
Context::Context() : mGLFW(nullptr)
{}

/// 
/// 
/// 
Context::~Context()
{
    if (mGLFW != nullptr)
    {
        glfwDestroyWindow(mGLFW);
    }
}

///
///
///
void Context::Initialize(Window& shared)
{
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    mGLFW = glfwCreateWindow((int)shared.GetWidth(), (int)shared.GetHeight(), "", nullptr, shared.get());

    if (mGLFW == nullptr)
    {
        LOG_ERROR("Failed to initialize shared context");
        return;
    }
}

/// 
/// 
/// 
void Context::Activate()
{
    std::unique_lock<std::mutex> lock{mActiveMutex};
    assert(!mActive);

    glfwMakeContextCurrent(mGLFW);
    mActive = true;
}

///
///
/// 
void Context::Deactivate()
{
    std::unique_lock<std::mutex> lock{mActiveMutex};
    assert(mActive);

    glfwMakeContextCurrent(nullptr);
    mActive = false;
}

}; // namespace Engine

}; // namespace CubeWorld
