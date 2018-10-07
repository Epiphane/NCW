// By Thomas Steinke

#pragma once

#include <memory>
#include <vector>

#include <Engine/UI/UIRoot.h>

namespace CubeWorld
{

namespace Editor
{

//
// Allows for easily swapping among multiple UIs.
//
class UISwapper
{
public:
   UISwapper()
      : mCurrent(nullptr)
      , mChildren{}
   {}

   //
   // Add a UIRoot to the swapper
   // 
   Engine::UIRoot* AddChild(std::unique_ptr<Engine::UIRoot>&& root)
   {
      Engine::UIRoot* ptr = root.get();
      mChildren.push_back(std::move(root));
      return ptr;
   }

   //
   // Construct a UIRoot and add it.
   //
   template <typename E, typename ...Args>
   E* Add(Args&& ...args)
   {
      static_assert(std::is_base_of<Engine::UIRoot, E>::value, "Only subclasses of UIRoot may be added to a UISwapper");
      return static_cast<E*>(AddChild(std::make_unique<E>(std::forward<Args>(args) ...)));
   }

   Engine::UIRoot* GetCurrent() { return mCurrent; }

   //
   // Swap to another UIElement
   //
   void Swap(Engine::UIRoot* window);

private:
   Engine::UIRoot* mCurrent;
   std::vector<std::unique_ptr<Engine::UIRoot>> mChildren;
};

}; // namespace Editor

}; // namespace CubeWorld
