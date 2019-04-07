// By Thomas Steinke

#pragma once

#include <stdint.h>

#include <RGBDesignPatterns/Command.h>
#include <Shared/UI/Swapper.h>

namespace CubeWorld
{

namespace Editor
{

class NavigateCommand : public Command
{
public:
   NavigateCommand(UI::Swapper* swapper, Engine::UIRoot* state)
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
   UI::Swapper* swapper;
   Engine::UIRoot* state;
   Engine::UIRoot* prev;
};

}; // namespace Editor

}; // namespace CubeWorld
