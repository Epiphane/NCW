// By Thomas Steinke

#pragma once

namespace CubeWorld
{

namespace Engine
{

template<typename T>
class scope_exit
{
public:
   explicit scope_exit(T&& exitScope) : exitScope(std::forward<T>(exitScope)) {};
   ~scope_exit() { try { exitScope(); } catch (...) {} }
private:
   T exitScope;
};

template<typename T>
scope_exit<T> create_scope_exit(T&& exitScope)
{
   return scope_exit<T>(std::forward<T>(exitScope));
}

}; // namespace Engine

}; // namespace CubeWorld

#define _CUBEWORLD_SCOPE_EXIT_LINENAME_IN(name, line) name##line
#define _CUBEWORLD_SCOPE_EXIT_LINENAME(name, line) _CUBEWORLD_SCOPE_EXIT_LINENAME_IN(name, line)
#define CUBEWORLD_SCOPE_EXIT(fn) const auto& _CUBEWORLD_SCOPE_EXIT_LINENAME(EXIT, __LINE__) = ::CubeWorld::Engine::create_scope_exit(fn); \
   _CUBEWORLD_SCOPE_EXIT_LINENAME(EXIT, __LINE__);