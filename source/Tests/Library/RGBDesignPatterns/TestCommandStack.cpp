// By Thomas Steinke

#include "../../catch.h"

#include <RGBDesignPatterns/CommandStack.h>

namespace CubeWorld
{

//
// Tests a simple operation
//
class IncrementCommand : public Command
{
public:
   IncrementCommand(int* value, int amount) : value(value), amount(amount) {};

   void Do() override { *value += amount; }
   void Undo() override { *value -= amount; }

private:
   int* value;
   int amount;
};

//
// Test ReverseCommand
//
using DecrementCommand = ReverseCommand<IncrementCommand>;

SCENARIO("CommandStack manipulates commands as expected") {

   GIVEN("An empty CommandStack") {
      CommandStack stack;
      int value = 0;

      REQUIRE(stack.empty());

      WHEN("Undo and redo are performed") {
         THEN("Nothing happens") {
            stack.Undo();
            CHECK(stack.empty());
            CHECK(value == 0);

            stack.Redo();
            CHECK(stack.empty());
            CHECK(value == 0);
         }
      }

      WHEN("A command is done") {
         stack.Do<IncrementCommand>(&value, 1);

         THEN("The value is incremented") {
            CHECK(!stack.empty());
            CHECK(value == 1);
         }

         THEN("It can be undone") {
            stack.Undo();
            CHECK(stack.empty());
            CHECK(value == 0);

            AND_THEN("It can be done again") {
               stack.Redo();
               CHECK(!stack.empty());
               CHECK(value == 1);
            }

            AND_THEN("New commands clear the stack") {
               stack.Do<IncrementCommand>(&value, 10);
               CHECK(!stack.empty());
               CHECK(value == 10);

               // The original command should be lost to the wind.
               stack.Redo();
               CHECK(value == 10);
            }
         }
      }

      WHEN("A reverse command is done") {
         value = 1;
         stack.Do<DecrementCommand>(&value, 1);

         THEN("The value is decremented") {
            CHECK(!stack.empty());
            CHECK(value == 0);

            AND_THEN("Undoing it does the original command") {
               stack.Undo();
               CHECK(stack.empty());
               CHECK(value == 1);
            }
         }
      }

      WHEN("Two commands are performed in order") {
         stack.Do<IncrementCommand>(&value, 1);
         REQUIRE(value == 1);
         stack.Do<IncrementCommand>(&value, 2);
         REQUIRE(value == 3);

         THEN("They are undone in order") {
            stack.Undo();
            CHECK(!stack.empty());
            CHECK(value == 1);

            stack.Undo();
            CHECK(stack.empty());
            CHECK(value == 0);

            AND_THEN("They are redone in order") {
               stack.Redo();
               CHECK(!stack.empty());
               CHECK(value == 1);

               stack.Redo();
               CHECK(value == 3);
            }
         }
      }

      WHEN("A command is emplaced in the stack") {
         stack.Emplace<IncrementCommand>(&value, 1);
         value++; // Increment externally instead

         THEN("It is not done") {
            CHECK(value == 1);
         }

         THEN("The stack is not empty") {
            CHECK(!stack.empty());
         }

         THEN("It can be undone") {
            stack.Undo();
            CHECK(stack.empty());
            CHECK(value == 0);

            AND_THEN("It can be done again") {
               stack.Redo();
               CHECK(!stack.empty());
               CHECK(value == 1);
            }

            AND_THEN("Emplacing new commands clear the stack") {
               stack.Emplace<IncrementCommand>(&value, 10);
               value += 10; // Increment externally instead
               CHECK(!stack.empty());
               CHECK(value == 10);

               // The original command should be lost to the wind.
               stack.Redo();
               CHECK(value == 10);
            }
         }
      }
   }

   GIVEN("A CommandStack with commands") {
      CommandStack stack;
      int value = 0;
      stack.Do<IncrementCommand>(&value, 1);
      stack.Do<IncrementCommand>(&value, 2);
      stack.Do<IncrementCommand>(&value, 3);

      REQUIRE(!stack.empty());
      REQUIRE(value == 6);

      WHEN("The stack is cleared") {
         THEN("it is empty") {
            stack.clear();
            CHECK(stack.empty());
            CHECK(value == 6);
         }
      }
   }
}

}; // namespace CubeWorld
