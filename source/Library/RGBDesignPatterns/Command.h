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

//
// Convenience class for creating a reversed command.
// e.g. given a DoThing command
//
// using DoReverseThing = ReverseCommand<DoThing>;
//
// DoReverseThing::Do -> DoThing::Undo
// DoReverseThing::Undo -> DoThing::Do
//
template <typename C>
class ReverseCommand : public C
{
public:
   using C::C;
   void Do() override { C::Undo(); }
   void Undo() override { C::Do(); }
};

}; // namespace CubeWorld
