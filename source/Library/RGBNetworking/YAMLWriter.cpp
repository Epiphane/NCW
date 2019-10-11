// By Thomas Steinke

#include <yaml.h>

#include <RGBText/Format.h>
#include "YAMLWriter.h"

namespace CubeWorld
{

const yaml_char_t* YAMLWriter::kNull = (const yaml_char_t*)"null";
const yaml_char_t* YAMLWriter::kTrue = (const yaml_char_t*)"true";
const yaml_char_t* YAMLWriter::kFalse = (const yaml_char_t*)"false";
const yaml_char_t* YAMLWriter::kZero = (const yaml_char_t*)"0";

bool YAMLWriter::Null()
{
   yaml_event_t event;
   yaml_scalar_event_initialize(&event, nullptr, nullptr, (yaml_char_t*)kNull, 4, 1, 1, YAML_PLAIN_SCALAR_STYLE);
   return yaml_emitter_emit(&mEmitter, &event) != 0;
}

bool YAMLWriter::Bool(bool b)
{
   yaml_event_t event;
   if (b)
   {
      yaml_scalar_event_initialize(&event, nullptr, nullptr, (yaml_char_t*)kTrue, 4, 1, 1, YAML_PLAIN_SCALAR_STYLE);
   }
   else
   {
      yaml_scalar_event_initialize(&event, nullptr, nullptr, (yaml_char_t*)kFalse, 5, 1, 1, YAML_PLAIN_SCALAR_STYLE);
   }
   return yaml_emitter_emit(&mEmitter, &event) != 0;
}

bool YAMLWriter::Int(int i)
{
   return Int64(i);
}

bool YAMLWriter::Uint(unsigned i)
{
   return Uint64(i);
}

bool YAMLWriter::Int64(int64_t i)
{
   if (mHolding.IsArray())
   {
      mHolding.push_back(i);
      return true;
   }

   if (i == 0)
   {
      yaml_event_t event;
      yaml_scalar_event_initialize(&event, nullptr, nullptr, (yaml_char_t*)kZero, 1, 1, 1, YAML_PLAIN_SCALAR_STYLE);
      return yaml_emitter_emit(&mEmitter, &event) != 0;
   }
   else
   {
      std::string formatted = FormatString("{}", i);
      return String(formatted.c_str(), (rapidjson::SizeType)formatted.size(), true);
   }
}

bool YAMLWriter::Uint64(uint64_t i)
{
   if (mHolding.IsArray())
   {
      mHolding.push_back(i);
      return true;
   }

   if (i == 0)
   {
      yaml_event_t event;
      yaml_scalar_event_initialize(&event, nullptr, nullptr, (yaml_char_t*)kZero, 1, 1, 1, YAML_PLAIN_SCALAR_STYLE);
      return yaml_emitter_emit(&mEmitter, &event) != 0;
   }
   else
   {
      std::string formatted = FormatString("{}", i);
      return String(formatted.c_str(), (rapidjson::SizeType)formatted.size(), true);
   }
}

bool YAMLWriter::Double(double d)
{
   if (mHolding.IsArray())
   {
      mHolding.push_back(d);
      return true;
   }

   if (d == 0)
   {
      yaml_event_t event;
      yaml_scalar_event_initialize(&event, nullptr, nullptr, (yaml_char_t*)kZero, 1, 1, 1, YAML_PLAIN_SCALAR_STYLE);
      return yaml_emitter_emit(&mEmitter, &event) != 0;
   }
   else
   {
      std::string formatted = std::to_string(d);
      size_t lastNonZero = formatted.find_last_not_of('0');
      if (formatted.at(lastNonZero) == '.')
      {
         --lastNonZero;
      }
      return String(formatted.c_str(), (rapidjson::SizeType)lastNonZero + 1, true);
   }
}

bool YAMLWriter::String(const Ch* str, SizeType length, bool /*copy*/)
{
   if (mHolding.IsArray() && !FlushArray(false))
   {
      return false;
   }

   yaml_event_t event;
   yaml_scalar_event_initialize(&event, nullptr, nullptr, (yaml_char_t*)str, (int)length, 1, 1, YAML_PLAIN_SCALAR_STYLE);
   return yaml_emitter_emit(&mEmitter, &event) != 0;
}

bool YAMLWriter::StartObject()
{
   if (mHolding.IsArray() && !FlushArray(false))
   {
      return false;
   }

   yaml_event_t event;
   yaml_mapping_start_event_initialize(&event, nullptr, nullptr, 0, YAML_BLOCK_MAPPING_STYLE);
   return yaml_emitter_emit(&mEmitter, &event) != 0;
}

bool YAMLWriter::Key(const Ch* str, SizeType length, bool copy)
{
   return String(str, length, copy);
}

bool YAMLWriter::EndObject(SizeType)
{
   yaml_event_t event;
   yaml_mapping_end_event_initialize(&event);
   return yaml_emitter_emit(&mEmitter, &event) != 0;
}

bool YAMLWriter::StartArray()
{
   mHolding.SetArray();
   return true;
}

bool YAMLWriter::FlushArray(bool condensed)
{
   yaml_event_t event;
   yaml_sequence_start_event_initialize(&event, nullptr, nullptr, 0,
      condensed ? YAML_FLOW_SEQUENCE_STYLE : YAML_BLOCK_SEQUENCE_STYLE);
   bool ret = yaml_emitter_emit(&mEmitter, &event) != 0;
   if (!ret) { return false; }

   BindingProperty holding = std::move(mHolding);
   mHolding.SetNull();
   for (const BindingProperty& val : holding)
   {
      if (!val.Write(*this))
      {
         return false;
      }
   }

   return true;
}

bool YAMLWriter::EndArray(SizeType)
{
   if (mHolding.IsArray())
   {
      FlushArray(true);
   }

   yaml_event_t event;
   yaml_sequence_end_event_initialize(&event);
   return yaml_emitter_emit(&mEmitter, &event) != 0;
}

}; // namespace CubeWorld
