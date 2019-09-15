// By Thomas Steinke

#pragma once

#include <memory>
#include <stack>

#include "Command.h"
#include "Singleton.h"

namespace CubeWorld
{

class CommandStack : public Singleton<CommandStack>
{
public:
   CommandStack() 
      : commands{}
      , undoneCommands{}
   {};

   void Do(std::unique_ptr<Command>&& command)
   {
      while (!undoneCommands.empty())
      {
         undoneCommands.pop();
      }

      command->Do();
      commands.push(std::move(command));
   }

   template<typename C, typename ...Args>
   void Do(Args ...args)
   {
      Do(std::make_unique<C>(std::forward<Args>(args) ...));
   }

   //
   // Add a command to the stack, as if it had already been done.
   //
   void Emplace(std::unique_ptr<Command>&& command)
   {
      while (!undoneCommands.empty())
      {
         undoneCommands.pop();
      }

      commands.push(std::move(command));
   }

   template<typename C, typename ...Args>
   void Emplace(Args ...args)
   {
      Emplace(std::make_unique<C>(std::forward<Args>(args) ...));
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

      std::unique_ptr<Command> command = std::move(undoneCommands.top());
      undoneCommands.pop();
      command->Do();
      commands.push(std::move(command));
   }

   bool empty() { return commands.empty(); }

private:
   std::stack<std::unique_ptr<Command>> commands;
   std::stack<std::unique_ptr<Command>> undoneCommands;
};

}; // namespace CubeWorld
