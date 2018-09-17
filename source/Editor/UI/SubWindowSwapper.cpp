// By Thomas Steinke

#include <cassert>

#include "SubWindowSwapper.h"

namespace CubeWorld
{

namespace Editor
{

void SubWindowSwapper::Swap(SubWindow* window)
{
   if (current != nullptr)
   {
      current->SetActive(false);
   }
   current = window;
   if (current != nullptr)
   {
      current->SetActive(true);
   }
}

}; // namespace Editor

}; // namespace CubeWorld
