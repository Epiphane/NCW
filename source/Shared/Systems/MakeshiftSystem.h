// By Thomas Steinke

#pragma once

#include <functional>

#include <Engine/Core/Input.h>
#include <Engine/System/System.h>

namespace CubeWorld
{

struct Makeshift : public Engine::Component<Makeshift> {
   using Callback = std::function<void(Engine::EntityManager&, Engine::EventManager&, TIMEDELTA)>;

   Makeshift(Callback cb) : callback(cb)
   {};
   
   Callback callback;
};

class MakeshiftSystem : public Engine::System<MakeshiftSystem> {
public:
   MakeshiftSystem() {}
   ~MakeshiftSystem() {}
   
   void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
};

}; // namespace CubeWorld
