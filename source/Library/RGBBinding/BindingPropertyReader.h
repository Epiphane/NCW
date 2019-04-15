// By Thomas Steinke

#pragma once

#include <stack>
#include <rapidjson/document.h>

#include "BindingProperty.h"

namespace CubeWorld
{

class BindingPropertyReader
{
public:
   BindingPropertyReader();

   const BindingProperty Read(const std::string& buffer);
   const BindingProperty GetResult() const { return data; }

public:
   // rapidjson::Handler implementation
   typedef char Ch;
   typedef rapidjson::SizeType SizeType;
   bool Null();
   bool Bool(bool b);
   bool Int(int i);
   bool Uint(unsigned i);
   bool Int64(int64_t i);
   bool Uint64(uint64_t i);
   bool Double(double d);
   bool RawNumber(const Ch* /*str*/, SizeType /*length*/, bool /*copy*/) { return false; }
   bool String(const Ch* str, SizeType length, bool copy);
   bool StartObject();
   bool Key(const Ch* str, SizeType length, bool copy);
   bool EndObject(SizeType memberCount);
   bool StartArray();
   bool EndArray(SizeType elementCount);

private:
   BindingProperty data;
   std::vector<BindingProperty*> cursor;
};

}; // namespace CubeWorld
