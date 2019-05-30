// By Thomas Steinke

#pragma once

#include <rapidjson/rapidjson.h>
#include <yaml.h>

#include <RGBBinding/BindingProperty.h>

namespace CubeWorld
{

class YAMLWriter
{
private:
   static const yaml_char_t* kNull;
   static const yaml_char_t* kTrue;
   static const yaml_char_t* kFalse;
   static const yaml_char_t* kZero;

public:
   YAMLWriter(yaml_emitter_t& emitter) : mEmitter(emitter) {};

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
   bool EndObject(SizeType memberCount = 0);
   bool StartArray();
   bool EndArray(SizeType elementCount = 0);

   // Helper function
   bool FlushArray(bool condensed);

private:
   yaml_emitter_t& mEmitter;

   BindingProperty mHolding;
};

}; // namespace CubeWorld
