// By Thomas Steinke

#pragma once

#include <stdint.h>

#include <Engine/Core/Command.h>

#include "../UI/SubWindowSwapper.h"

namespace CubeWorld
{

namespace Editor
{

class NavigateCommand : public Command
{
public:
   NavigateCommand(SubWindowSwapper* swapper, SubWindow* state)
      : swapper(swapper)
      , state(state)
      , prev(nullptr)
   {};

   void Do() override
   {
      prev = swapper->GetCurrent();
      swapper->Swap(state);
   }
   void Undo() override
   {
      swapper->Swap(prev);
   }

private:
   SubWindowSwapper* swapper;
   SubWindow* state;
   SubWindow* prev;
};

}; // namespace Editor

}; // namespace CubeWorld
