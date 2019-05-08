// By Thomas Steinke

#include "BindingPropertyReader.h"

namespace CubeWorld
{

BindingPropertyReader::BindingPropertyReader()
{
   data.SetNull();
   cursor.clear();
   cursor.push_back(&data);
}

const BindingProperty BindingPropertyReader::Read(const std::string& buffer)
{
   rapidjson::GenericStringStream<rapidjson::UTF8<>> stream(buffer.c_str());
   rapidjson::GenericReader<rapidjson::UTF8<>, rapidjson::UTF8<>> reader;

   reader.Parse(stream, *this);
   return data;
}

bool BindingPropertyReader::Null()
{
   if (cursor.empty()) { return false; }
   BindingProperty* prop = cursor.back();
   if (prop->IsArray())
   {
      prop->PushBack(BindingProperty());
   }
   else
   {
      cursor.back()->SetNull();
      cursor.pop_back();
   }
   return true;
}

bool BindingPropertyReader::Bool(bool b)
{
   if (cursor.empty()) { return false; }
   BindingProperty* prop = cursor.back();
   if (prop->IsArray())
   {
      prop->PushBack(BindingProperty(b));
   }
   else
   {
      cursor.back()->SetBool(b);
      cursor.pop_back();
   }
   return true;
}

bool BindingPropertyReader::Int(int i)
{
   if (cursor.empty()) { return false; }
   BindingProperty* prop = cursor.back();
   if (prop->IsArray())
   {
      prop->PushBack(BindingProperty(i));
   }
   else
   {
      cursor.back()->SetInt(i);
      cursor.pop_back();
   }
   return true;
}

bool BindingPropertyReader::Uint(unsigned i)
{
   if (cursor.empty()) { return false; }
   BindingProperty* prop = cursor.back();
   if (prop->IsArray())
   {
      prop->PushBack(BindingProperty(i));
   }
   else
   {
      cursor.back()->SetUint(i);
      cursor.pop_back();
   }
   return true;
}

bool BindingPropertyReader::Int64(int64_t i)
{
   if (cursor.empty()) { return false; }
   BindingProperty* prop = cursor.back();
   if (prop->IsArray())
   {
      prop->PushBack(BindingProperty(i));
   }
   else
   {
      cursor.back()->SetInt64(i);
      cursor.pop_back();
   }
   return true;
}

bool BindingPropertyReader::Uint64(uint64_t i)
{
   if (cursor.empty()) { return false; }
   BindingProperty* prop = cursor.back();
   if (prop->IsArray())
   {
      prop->PushBack(BindingProperty(i));
   }
   else
   {
      cursor.back()->SetUint64(i);
      cursor.pop_back();
   }
   return true;
}

bool BindingPropertyReader::Double(double d)
{
   if (cursor.empty()) { return false; }
   BindingProperty* prop = cursor.back();
   if (prop->IsArray())
   {
      prop->PushBack(BindingProperty(d));
   }
   else
   {
      cursor.back()->SetDouble(d);
      cursor.pop_back();
   }
   return true;
}

bool BindingPropertyReader::String(const Ch* str, SizeType length, bool /*copy*/)
{
   if (cursor.empty()) { return false; }
   BindingProperty* prop = cursor.back();
   if (prop->IsArray())
   {
      prop->PushBack(BindingProperty({str, length}));
   }
   else
   {
      cursor.back()->SetString({str, length});
      cursor.pop_back();
   }
   return true;
}

bool BindingPropertyReader::StartObject()
{
   if (cursor.empty()) { return false; }
   BindingProperty* prop = cursor.back();
   if (prop->IsArray())
   {
      cursor.push_back(&prop->push_back(BindingProperty(BindingProperty::kObjectType)));
   }
   else
   {
      cursor.back()->SetObject();
   }
   return true;
}

bool BindingPropertyReader::Key(const Ch* str, SizeType length, bool /*copy*/)
{
   if (cursor.empty()) { return false; }
   if (!cursor.back()->IsObject()) { return false; }
   BindingProperty* obj = cursor.back();
   cursor.push_back(&obj->Set({str, length}, BindingProperty{}));
   return true;
}

bool BindingPropertyReader::EndObject(SizeType /*memberCount*/)
{
   if (cursor.empty()) { return false; }
   if (!cursor.back()->IsObject()) { return false; }
   cursor.pop_back();
   return true;
}

bool BindingPropertyReader::CurrentIsObject()
{
   return cursor.back()->IsObject();
}

bool BindingPropertyReader::StartArray()
{
   if (cursor.empty()) { return false; }
   cursor.back()->SetArray();
   return true;
}

bool BindingPropertyReader::EndArray(SizeType /*elementCount*/)
{
   if (cursor.empty()) { return false; }
   if (!cursor.back()->IsArray()) { return false; }
   cursor.pop_back();
   return true;
}

}; // namespace CubeWorld
