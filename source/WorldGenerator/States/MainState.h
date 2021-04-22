// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Shared/Event/NamedEvent.h>
#include "../World/World.h"

namespace CubeWorld
{

class MainState : public Engine::State, public Engine::Receiver<MainState> {
public:
    MainState(Engine::Input* input, Bounded& parent);
    ~MainState();

    void Initialize() override;

    void SetParent(Engine::EventManager* other) { mEvents.SetParent(other); }
    template<typename E>
    void TransformParentEvents(Engine::Transformer<E>* transformer)
    {
        mEvents.TransformParentEvents<E>(transformer);
    }

private:
    std::unique_ptr<Engine::Input::KeyCallbackLink> mDebugCallback;

    Engine::Graphics::CameraHandle mCamera;

    World mWorld;

    Engine::Input* mInput;
    Bounded& mParent;
};

}; // namespace CubeWorld
