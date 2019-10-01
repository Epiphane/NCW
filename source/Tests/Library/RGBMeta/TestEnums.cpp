// By Thomas Steinke

#include "../../catch.h"

#include <Meta.h>
#include <RGBMeta/Value.h>

namespace meta
{

enum class MyEnum
{
   A, B, C
};

template<>
inline auto registerValues<MyEnum>()
{
   return values(
      value("A", MyEnum::A),
      value("B", MyEnum::B),
      value("C", MyEnum::C)
   );
}

}; // namespace meta

namespace CubeWorld
{

SCENARIO("Enum definition and iteration") {
   GIVEN("A predefined enum") {
      WHEN("an enum value is iterated over") {
         std::vector<std::pair<std::string, meta::MyEnum>> outputs{};

         meta::doForAllValues<meta::MyEnum>([&](const auto& item) {
            outputs.push_back(std::make_pair(item.getName(), item.getValue()));
         });

         THEN("All values are provided to the function, in order") {

            REQUIRE(outputs.size() == 3);
            CHECK(outputs[0].first == "A");
            CHECK(outputs[0].second == meta::MyEnum::A);
            CHECK(outputs[1].first == "B");
            CHECK(outputs[1].second == meta::MyEnum::B);
            CHECK(outputs[2].first == "C");
            CHECK(outputs[2].second == meta::MyEnum::C);

         }
      }

      WHEN("an enum value is searched for") {
         THEN("it is found") {
            CHECK(meta::getValue<meta::MyEnum>("A") == meta::MyEnum::A);
            CHECK(meta::getValue<meta::MyEnum>("B") == meta::MyEnum::B);
            CHECK(meta::getValue<meta::MyEnum>("C") == meta::MyEnum::C);
         }
      }

      WHEN("an enum name is searched for") {
         THEN("it is found") {
            CHECK(meta::getName<meta::MyEnum>(meta::MyEnum::A) == "A");
            CHECK(meta::getName<meta::MyEnum>(meta::MyEnum::B) == "B");
            CHECK(meta::getName<meta::MyEnum>(meta::MyEnum::C) == "C");
         }
      }
   }
}

}; // namespace CubeWorld
