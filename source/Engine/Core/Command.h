// By Thomas Steinke

#pragma once

namespace CubeWorld
{

class Command
{
public:
   virtual ~Command() {}
   virtual void Do() = 0;
   virtual void Undo() = 0;
};

}; // namespace CubeWorld
