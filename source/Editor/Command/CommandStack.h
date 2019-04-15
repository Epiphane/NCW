// By Thomas Steinke

#pragma once

#include <memory>
#include <stack>

#include <RGBDesignPatterns/Command.h>
#include <RGBDesignPatterns/Singleton.h>
#include <Engine/Event/Event.h>

namespace CubeWorld
{

namespace Editor
{

class CommandStack : public Engine::Singleton<CommandStack>
{
public:
   CommandStack() 
      : commands{}
      , undoneCommands{}
   {};

   void Do(std::unique_ptr<Command>&& command)
   {
      command->Do();
      commands.push(std::move(command));
   }

   template<typename C, typename ...Args>
   void Do(Args ...args)
   {
      Do(std::make_unique<C>(std::forward<Args>(args) ...));
   }

   void Undo()
   {
      if (commands.empty())
      {
         return;
      }

      std::unique_ptr<Command> undo = std::move(commands.top());
      commands.pop();
      undo->Undo();
      undoneCommands.push(std::move(undo));
   }

   void Redo()
   {
      if (undoneCommands.empty())
      {
         return;
      }

      Do(std::move(undoneCommands.top()));
      undoneCommands.pop();
   }

   bool empty() { return commands.empty(); }

private:
   std::stack<std::unique_ptr<Command>> commands;
   std::stack<std::unique_ptr<Command>> undoneCommands;
};

}; // namespace Editor

}; // namespace CubeWorld
