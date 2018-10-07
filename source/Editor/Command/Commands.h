// By Thomas Steinke

#pragma once

#include <stdint.h>

#include <Engine/Core/Command.h>

#include "../UI/UISwapper.h"

namespace CubeWorld
{

namespace Editor
{

class NavigateCommand : public Command
{
public:
   NavigateCommand(UISwapper* swapper, Engine::UIRoot* state)
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
   UISwapper* swapper;
   Engine::UIRoot* state;
   Engine::UIRoot* prev;
};

}; // namespace Editor

}; // namespace CubeWorld
