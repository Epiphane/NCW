// By Thomas Steinke

#include "../../catch.h"

#include <glm/glm.hpp>
#include <Meta.h>

#include <RGBBinding/BindingProperty.h>
#include <RGBMeta/Value.h>
#include <Engine/Entity/Component.h>

namespace meta
{

enum class MyEnum
{
   A, B, C
};

struct MySubStruct : public CubeWorld::Engine::Component<MySubStruct>
{
#pragma warning (disable: 4582 4583)
   MySubStruct() : type(MyEnum::A) { a = 0; };
   MySubStruct(const MySubStruct& other) : type(other.type)
   {
      if (type == MyEnum::A) { a = other.a; }
      else if (type == MyEnum::B) { new (&b) std::string(other.b); }
      else if (type == MyEnum::C) { c = other.c; }
   }
   ~MySubStruct() { FreeValue(); };
#pragma warning (default: 4582 4583)

   MySubStruct& operator=(const MySubStruct& other)
   {
      FreeValue();

      type = other.type;
      if (type == MyEnum::A) { a = other.a; }
      else if (type == MyEnum::B) { new (&b) std::string(other.b); }
      else if (type == MyEnum::C) { c = other.c; }

      return *this;
   }

   // Frees the current value, with the expectation that the type
   // will be changed.
   void FreeValue() { if (IsB()) { b.~basic_string(); } }

   MyEnum type;
   union {
      int a;
      std::string b;
      glm::vec3 c;
   };

   inline void SetA(const int& a_)         { FreeValue(); type = MyEnum::A; a = a_; }
   inline void SetB(const std::string& b_) { FreeValue(); type = MyEnum::B; new (&b) std::string(b_); }
   inline void SetC(const glm::vec3& c_)   { FreeValue(); type = MyEnum::C; c = c_; }
   const inline int&          GetA() const { return a; }
   const inline std::string&  GetB() const { return b; }
   const inline glm::vec3&    GetC() const { return c; }
   const inline bool IsA() const { return type == MyEnum::A; }
   const inline bool IsB() const { return type == MyEnum::B; }
   const inline bool IsC() const { return type == MyEnum::C; }
};

struct MyStruct
{
   double val;
   std::vector<MySubStruct> substructs;
   std::unordered_map<std::string, std::string> properties;
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

template<>
inline auto registerMembers<MySubStruct>()
{
   return members(
      //member("type", &MySubStruct::type),
      member("aVal", &MySubStruct::GetA, &MySubStruct::SetA, &MySubStruct::IsA),
      member("bVal", &MySubStruct::GetB, &MySubStruct::SetB, &MySubStruct::IsB),
      member("cVal", &MySubStruct::GetC, &MySubStruct::SetC, &MySubStruct::IsC)
   );
}

template<>
inline auto registerMembers<MyStruct>()
{
   return members(
      member("val", &MyStruct::val),
      member("substructs", &MyStruct::substructs),
      member("properties", &MyStruct::properties)
   );
}

}; // namespace meta

namespace CubeWorld
{

SCENARIO("BindingProperty serialization") {
   GIVEN("A created struct") {
      meta::MyStruct myStruct;
      meta::MySubStruct subStruct;

      myStruct.val = 10.0;

      subStruct.SetA(10);
      myStruct.substructs.push_back(subStruct);

      subStruct.SetB("Test String");
      myStruct.substructs.push_back(subStruct);

      subStruct.SetC({1, 2, 3});
      myStruct.substructs.push_back(subStruct);

      myStruct.properties["a"] = "b";
      myStruct.properties["c"] = "d";

      WHEN("The structure is serialized") {
         BindingProperty serialized = Binding::serialize(myStruct);

         THEN("Simple values are translated") {
            CHECK(serialized["val"].GetDoubleValue() == 10.0);
         }

         THEN("SubStructs are serialized") {
            REQUIRE(serialized["substructs"].IsArray());
            REQUIRE(serialized["substructs"].GetSize() == 3);

            CHECK( serialized["substructs"][0].Has("aVal"));
            CHECK(!serialized["substructs"][0].Has("bVal"));
            CHECK(!serialized["substructs"][0].Has("cVal"));
            CHECK( serialized["substructs"][0]["aVal"].GetIntValue() == 10);

            CHECK(!serialized["substructs"][1].Has("aVal"));
            CHECK( serialized["substructs"][1].Has("bVal"));
            CHECK(!serialized["substructs"][1].Has("cVal"));
            CHECK( serialized["substructs"][1]["bVal"].GetStringValue() == "Test String");

            CHECK(!serialized["substructs"][2].Has("aVal"));
            CHECK(!serialized["substructs"][2].Has("bVal"));
            CHECK( serialized["substructs"][2].Has("cVal"));
            CHECK( serialized["substructs"][2]["cVal"].GetVec3() == glm::vec3{1, 2, 3});
         }

         THEN("Maps are serialized") {
            CHECK(serialized["properties"].IsObject());
            CHECK(serialized["properties"].GetSize() == 2);
            CHECK(serialized["properties"]["a"] == "b");
            CHECK(serialized["properties"]["c"] == "d");
         }

         THEN("It can be deserialized") {
            meta::MyStruct deserialized;
            Binding::deserialize(deserialized, serialized);

            AND_THEN("Simple values are correct") {
               CHECK(deserialized.val == 10.0);
            }

            AND_THEN("SubStructs are correct") {
               REQUIRE(deserialized.substructs.size() == 3);

               CHECK(deserialized.substructs[0].IsA());
               CHECK(deserialized.substructs[0].GetA() == 10);

               CHECK(deserialized.substructs[1].IsB());
               CHECK(deserialized.substructs[1].GetB() == "Test String");

               CHECK(deserialized.substructs[2].IsC());
               CHECK(deserialized.substructs[2].GetC() == glm::vec3{1, 2, 3});
            }

            AND_THEN("Maps are correct") {
               REQUIRE(deserialized.properties.size() == 2);

               REQUIRE(deserialized.properties.find("a") != deserialized.properties.end());
               REQUIRE(deserialized.properties.find("c") != deserialized.properties.end());

               CHECK(deserialized.properties.at("a") == "b");
               CHECK(deserialized.properties.at("c") == "d");
            }
         }
      }
   }
}

}; // namespace CubeWorld
