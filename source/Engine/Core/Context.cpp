// By Thomas Steinke

#include <algorithm>

#include "Context.h"

namespace CubeWorld
{

namespace Engine
{

void ContextManager::Add(Context* ctx)
{
    mContexts.push_back(ctx);
}

void ContextManager::Remove(Context* ctx)
{
    auto _ = std::remove(mContexts.begin(), mContexts.end(), ctx);
}

std::vector<Context*> ContextManager::GetAll()
{
    return mContexts;
}

std::vector<Context*> sContexts;

/// 
/// 
/// 
Context::Context() : mGLFW(nullptr)
{
    ContextManager::Instance().Add(this);
}

/// 
/// 
/// 
Context::~Context()
{
    if (mGLFW != nullptr)
    {
        glfwDestroyWindow(mGLFW);
    }

    ContextManager::Instance().Remove(this);
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
