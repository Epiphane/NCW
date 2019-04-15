#include <stdio.h>

#include <RGBNetworking/JSONSerializer.h>

using namespace CubeWorld;

int main(int argc, char** argv)
{
   std::string buffer = R"({"key1":[{"key2":10}]})";

   Maybe<BindingProperty> deserialized = JSONSerializer::Deserialize(buffer);

   BindingProperty data = *deserialized;
   if (data["key"] == 10)
   {
      printf("abc\n");
   }

   printf("deserialized: %s\n", deserialized->begin()->GetStringValue().c_str());

   for (int i = 0; i < argc; i++)
   {
      printf("Arg: %s\n", argv[i]);
   }

   return 0;
}