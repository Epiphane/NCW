// By Thomas Steinke

#include <glm/gtc/epsilon.hpp>

#include "BindingProperty.h"

namespace CubeWorld
{

std::string BindingProperty::TypeToString(Type type)
{
    switch (type)
    {
    case Type::Null:
        return "Null";
    case Type::True:
        return "True";
    case Type::False:
        return "False";
    case Type::Number:
        return "Number";
    case Type::String:
        return "String";
    case Type::Object:
        return "Object";
    case Type::Array:
        return "Array";
    default:
        return "N/A";
    }
}

const BindingProperty BindingProperty::Null = BindingProperty{};
BindingProperty BindingProperty::_ = BindingProperty{};

///
///
///
BindingProperty::BindingProperty(const Type& type)
{
    static const uint16_t defaultFlags[7] = {
       kNullFlag,
       kFalseFlag,
       kTrueFlag,
       kNumberAnyFlag,
       kStringFlag,
       kObjectFlag,
       kArrayFlag,
    };

    flags = defaultFlags[size_t(type)];
    if (type == Type::String) { data.stringVal.clear(); }
    else if (type == Type::Object) { new (&data.objectVal) RGBBinding::Array<KeyVal>(); }
    else if (type == Type::Array) { new (&data.arrayVal) RGBBinding::Array<BindingProperty>(); }
    else { data.numVal.i64 = 0; }
}

BindingProperty::BindingProperty(const BindingProperty& other) noexcept
    : flags(other.flags)
{
    if (IsNumber()) { data.numVal = other.data.numVal; }
    else if (IsString()) { new (&data.stringVal) std::string(other.data.stringVal); }
    else if (IsObject()) { new (&data.objectVal) RGBBinding::Array<KeyVal>(other.data.objectVal); }
    else if (IsArray()) { new (&data.arrayVal) RGBBinding::Array<BindingProperty>(other.data.arrayVal); }
}

BindingProperty::BindingProperty(BindingProperty&& other) noexcept
    : flags(other.flags)
{
    if (IsNumber()) { data.numVal = other.data.numVal; }
    else if (IsString()) { new (&data.stringVal) std::string(std::move(other.data.stringVal)); }
    else if (IsObject()) { new (&data.objectVal) RGBBinding::Array<KeyVal>(std::move(other.data.objectVal)); }
    else if (IsArray()) { new (&data.arrayVal) RGBBinding::Array<BindingProperty>(std::move(other.data.arrayVal)); }
}

BindingProperty::~BindingProperty()
{
    if (IsNull())
    {
        return;
    }
    else if (IsString())
    {
        data.stringVal.~basic_string();
    }
    else if (IsObject())
    {
        data.objectVal.~Array();
    }
    else if (IsArray())
    {
        data.arrayVal.~Array();
    }
}

///
///
///
BindingProperty::BindingProperty(bool value)
{
    data.numVal.i64 = 0;
    flags = value ? kTrueFlag : kFalseFlag;
}

BindingProperty::BindingProperty(int32_t i)
{
    data.numVal.i64 = i;
    flags = uint16_t((i >= 0) ? (kNumberIntFlag | kUintFlag | kUint64Flag) : kNumberIntFlag);
}

BindingProperty::BindingProperty(int64_t i64)
{
    data.numVal.i64 = i64;
    flags = kNumberInt64Flag;
    if (i64 >= 0) {
        flags |= kNumberUint64Flag;
        if (!(static_cast<uint64_t>(i64) & RAPIDJSON_UINT64_C2(0xFFFFFFFF, 0x00000000)))
            flags |= kUintFlag;
        if (!(static_cast<uint64_t>(i64) & RAPIDJSON_UINT64_C2(0xFFFFFFFF, 0x80000000)))
            flags |= kIntFlag;
    }
    else if (i64 >= static_cast<int64_t>(RAPIDJSON_UINT64_C2(0xFFFFFFFF, 0x80000000)))
        flags |= kIntFlag;
}

BindingProperty::BindingProperty(uint32_t u)
{
    data.numVal.u64 = u;
    flags = uint16_t((u & 0x80000000) ? kNumberUintFlag : (kNumberUintFlag | kIntFlag | kInt64Flag));
}

BindingProperty::BindingProperty(uint64_t u64)
{
    data.numVal.u64 = u64;
    flags = kNumberUint64Flag;
    if (!(u64 & RAPIDJSON_UINT64_C2(0x80000000, 0x00000000)))
        flags |= kInt64Flag;
    if (!(u64 & RAPIDJSON_UINT64_C2(0xFFFFFFFF, 0x00000000)))
        flags |= kUintFlag;
    if (!(u64 & RAPIDJSON_UINT64_C2(0xFFFFFFFF, 0x80000000)))
        flags |= kIntFlag;
}

BindingProperty::BindingProperty(double d)
{
    data.numVal.d = d;
    flags = kNumberDoubleFlag;
}

BindingProperty::BindingProperty(float f)
{
    data.numVal.d = f;
    flags = kNumberDoubleFlag;
}

BindingProperty::BindingProperty(const char* s)
{
    new (&data.stringVal) std::string(s);
    flags = kStringFlag;
}

BindingProperty::BindingProperty(const std::string& s)
{
    new (&data.stringVal) std::string(s);
    flags = kStringFlag;
}

BindingProperty::BindingProperty(std::string&& s)
{
    new (&data.stringVal) std::string(std::move(s));
    flags = kStringFlag;
}

BindingProperty::BindingProperty(const glm::vec3& vec3) : BindingProperty(Type::Array)
{
    push_back(vec3[0]);
    push_back(vec3[1]);
    push_back(vec3[2]);
}

BindingProperty::BindingProperty(const glm::vec4& vec4) : BindingProperty(Type::Array)
{
    push_back(vec4[0]);
    push_back(vec4[1]);
    push_back(vec4[2]);
    push_back(vec4[3]);
}

///
///
///
BindingProperty& BindingProperty::operator=(const BindingProperty& other)
{
    this->~BindingProperty();
    new (this) BindingProperty(other);
    return *this;
}

BindingProperty& BindingProperty::operator=(BindingProperty&& other) noexcept
{
    this->~BindingProperty();
    new (this) BindingProperty(other);
    return *this;
}

BindingProperty& BindingProperty::operator[](const int& index)
{
    assert(index >= 0 && "Negative index is invalid");
    return this->operator[](size_t(index));
}

BindingProperty& BindingProperty::operator[](const size_t& index)
{
    assert((IsArray() || IsNull()) && "index operator only valid on null or arrays");
    if (IsNull()) { SetArray(); }
    if (index >= data.arrayVal.size())
    {
        data.arrayVal.resize(uint32_t(index + 1));
    }
    return data.arrayVal[uint32_t(index)];
}

BindingProperty& BindingProperty::operator[](const std::string& key)
{
    assert((IsObject() || IsNull()) && "key operator only valid on null or objects");
    if (IsNull()) { SetObject(); }
    ObjectIterator it = Find(key);
    if (it == AsObject().end())
    {
        data.objectVal.push_back(KeyVal{ key, BindingProperty{} });
    }
    return it->value;
}

BindingProperty& BindingProperty::operator[](const char* key)
{
    return this->operator[](std::string{ key });
}

const BindingProperty& BindingProperty::operator[](const int& index) const
{
    assert(index >= 0 && "Negative index is invalid");
    return this->operator[](size_t(index));
}

const BindingProperty& BindingProperty::operator[](const size_t& index) const
{
    assert((IsArray() || IsNull()) && "index operator only valid on null or arrays");
    if (index < data.arrayVal.size())
    {
        return data.arrayVal[uint32_t(index)];
    }
    return Null;
}

const BindingProperty& BindingProperty::operator[](const std::string& key) const
{
    assert((IsObject() || IsNull()) && "key operator only valid on null or objects");
    if (IsNull())
    {
        return Null;
    }

    ConstObjectIterator it = Find(key);
    if (it != AsObject().end())
    {
        return it->value;
    }
    return Null;
}

const BindingProperty& BindingProperty::operator[](const char* key) const
{
    return this->operator[](std::string{ key });
}

bool BindingProperty::operator==(const BindingProperty& other) const
{
    if (IsNumber() && other.IsNumber())
    {
        if (glm::epsilonEqual(GetDoubleValue(), other.GetDoubleValue(), DBL_EPSILON))
        {
            return true;
        }
    }

    if ((flags & kTypeMask) != (other.flags & kTypeMask))
    {
        return false;
    }

    if (IsDouble()) { return glm::epsilonEqual(data.numVal.d, other.data.numVal.d, DBL_EPSILON); }
    else if (IsNumber()) { return data.numVal.u64 == other.data.numVal.u64; }
    else if (IsString()) { return data.stringVal == other.data.stringVal; }
    else if (IsObject()) { return data.objectVal == other.data.objectVal; }
    else if (IsArray()) { return data.arrayVal == other.data.arrayVal; }
    return true;
}

///
///
///
bool BindingProperty::IsVec3() const
{
    return IsArray()
        && data.arrayVal.size() == 3
        && data.arrayVal[0].IsNumber()
        && data.arrayVal[1].IsNumber()
        && data.arrayVal[2].IsNumber();
}

bool BindingProperty::IsVec4() const
{
    return IsArray()
        && data.arrayVal.size() == 4
        && data.arrayVal[0].IsNumber()
        && data.arrayVal[1].IsNumber()
        && data.arrayVal[2].IsNumber()
        && data.arrayVal[3].IsNumber();
}

///
///
///
const bool BindingProperty::GetBooleanValue(const bool& defaultValue) const
{
    return (flags & kBoolFlag) != 0 ? flags == kTrueFlag : defaultValue;
}

const int32_t BindingProperty::GetIntValue(const int32_t& defaultValue) const
{
    return (flags & kIntFlag) != 0 ? data.numVal.i.i : defaultValue;
}

const uint32_t BindingProperty::GetUintValue(const uint32_t& defaultValue) const
{
    return (flags & kUintFlag) != 0 ? data.numVal.u.u : defaultValue;
}

const int64_t BindingProperty::GetInt64Value(const int64_t& defaultValue) const
{
    return (flags & kInt64Flag) != 0 ? data.numVal.i64 : defaultValue;
}

const uint64_t BindingProperty::GetUint64Value(const uint64_t& defaultValue) const
{
    return (flags & kUint64Flag) != 0 ? data.numVal.u64 : defaultValue;
}

const double BindingProperty::GetDoubleValue(const double& defaultValue) const
{
    if ((flags & kNumberFlag) == 0) return defaultValue;
    if ((flags & kDoubleFlag) != 0) return data.numVal.d;
    if ((flags & kIntFlag) != 0)    return data.numVal.i.i;
    if ((flags & kUintFlag) != 0)   return data.numVal.u.u;
    if ((flags & kInt64Flag) != 0)  return static_cast<double>(data.numVal.i64);
    if ((flags & kUint64Flag) != 0) return static_cast<double>(data.numVal.u64);
    return defaultValue;
}

const float BindingProperty::GetFloatValue(const float& defaultValue) const
{
    if ((flags & kNumberFlag) == 0) return defaultValue;
    if ((flags & kDoubleFlag) != 0) return static_cast<float>(data.numVal.d);
    if ((flags & kIntFlag) != 0)    return static_cast<float>(data.numVal.i.i);
    if ((flags & kUintFlag) != 0)   return static_cast<float>(data.numVal.u.u);
    if ((flags & kInt64Flag) != 0)  return static_cast<float>(data.numVal.i64);
    if ((flags & kUint64Flag) != 0) return static_cast<float>(data.numVal.u64);
    return defaultValue;
}

const std::string BindingProperty::GetStringValue(const std::string& defaultValue) const
{
    return flags == uint16_t(kStringFlag) ? data.stringVal : defaultValue;
}

glm::vec3 BindingProperty::GetVec3(const glm::vec3& defaultValue) const
{
    return IsVec3() ? glm::vec3{
       data.arrayVal[0].GetDoubleValue(),
       data.arrayVal[1].GetDoubleValue(),
       data.arrayVal[2].GetDoubleValue()
    } : defaultValue;
}

glm::vec4 BindingProperty::GetVec4(const glm::vec4& defaultValue) const
{
    return IsVec4() ? glm::vec4{
       data.arrayVal[0].GetDoubleValue(),
       data.arrayVal[1].GetDoubleValue(),
       data.arrayVal[2].GetDoubleValue(),
       data.arrayVal[3].GetDoubleValue()
    } : defaultValue;
}

BindingProperty::Object BindingProperty::AsObject()
{
    assert(IsObject() && "GetObject only works on an object");
    return Object(this);
}

BindingProperty::ConstObject BindingProperty::AsObject() const
{
    assert(IsObject() && "GetObject only works on an object");
    return ConstObject(this);
}

BindingProperty::Array BindingProperty::AsArray()
{
    assert(IsArray() && "GetArray only works on an array");
    return Array(this);
}

BindingProperty::ConstArray BindingProperty::AsArray() const
{
    assert(IsArray() && "GetArray only works on an array");
    return ConstArray(this);
}

///
///
///
BindingProperty& BindingProperty::PushBack(BindingProperty val)
{
    if (IsNull())
    {
        SetArray();
    }
    assert(IsArray() && "PushBack is only valid on an array");
    data.arrayVal.push_back(val);
    return data.arrayVal[data.arrayVal.size() - 1];
}

void BindingProperty::PopBack()
{
    if (IsArray())
    {
        data.arrayVal.pop_back();
    }
    if (IsObject())
    {
        data.objectVal.pop_back();
    }
}

size_t BindingProperty::GetSize() const
{
    if (IsArray())
    {
        return data.arrayVal.size();
    }
    if (IsObject())
    {
        return data.objectVal.size();
    }
    return 0;
}

///
///
///
bool BindingProperty::Has(const std::string& key) const
{
    if (IsNull())
    {
        return false;
    }

    assert(IsObject() && "Has is only valid on an object");
    ConstObject me = AsObject();
    ConstObjectIterator it = me.begin();
    ConstObjectIterator end = me.end();
    for (; it != end; ++it)
    {
        if (it->key == key)
        {
            return true;
        }
    }
    return false;
}

BindingProperty::ObjectIterator BindingProperty::Find(const std::string& key)
{
    assert(IsObject() && "Find is only valid on an object");
    Object me = AsObject();
    ObjectIterator it = me.begin();
    ObjectIterator end = me.end();
    for (; it != end; ++it)
    {
        if (it->key == key)
        {
            break;
        }
    }
    return it;
}

BindingProperty::ConstObjectIterator BindingProperty::Find(const std::string& key) const
{
    assert(IsObject() && "Find is only valid on an object");
    ConstObject me = AsObject();
    ConstObjectIterator it = me.begin();
    ConstObjectIterator end = me.end();
    for (; it != end; ++it)
    {
        if (it->key == key)
        {
            break;
        }
    }
    return it;
}

BindingProperty& BindingProperty::Set(const std::string& key, const BindingProperty& value)
{
    return Set(key, BindingProperty(value));
}

BindingProperty& BindingProperty::Set(const std::string& key, BindingProperty&& value)
{
    assert(IsObject() && "Set is only valid on an object");
    ObjectIterator it = Find(key);
    if (it == AsObject().end())
    {
        data.objectVal.push_back(KeyVal{ key, std::move(value) });
    }

    return it->value;
}

///
///
///
BindingProperty::Iterator BindingProperty::begin()
{
    return Iterator(this, 0);
}

BindingProperty::Iterator BindingProperty::end()
{
    switch (flags)
    {
    case kArrayFlag:
        return Iterator(this, data.arrayVal.size());
    case kObjectFlag:
        return Iterator(this, data.objectVal.size());
    default:
        return Iterator(this, 0);
    }
}

BindingProperty::ConstIterator BindingProperty::begin() const
{
    return ConstIterator(this, 0);
}

BindingProperty::ConstIterator BindingProperty::end() const
{
    switch (flags)
    {
    case kArrayFlag:
        return ConstIterator(this, data.arrayVal.size());
    case kObjectFlag:
        return ConstIterator(this, data.objectVal.size());
    default:
        return ConstIterator(this, 0);
    }
}

BindingProperty::PairIterator BindingProperty::begin_pairs()
{
    return PairIterator(this, 0);
}

BindingProperty::PairIterator BindingProperty::end_pairs()
{
    switch (flags)
    {
    case kArrayFlag:
        return PairIterator(this, data.arrayVal.size());
    case kObjectFlag:
        return PairIterator(this, data.objectVal.size());
    default:
        return PairIterator(this, 0);
    }
}

BindingProperty::ConstPairIterator BindingProperty::begin_pairs() const
{
    return ConstPairIterator(this, 0);
}

BindingProperty::ConstPairIterator BindingProperty::end_pairs() const
{
    switch (flags)
    {
    case kArrayFlag:
        return ConstPairIterator(this, data.arrayVal.size());
    case kObjectFlag:
        return ConstPairIterator(this, data.objectVal.size());
    default:
        return ConstPairIterator(this, 0);
    }
}

BindingProperty::ObjectIterator BindingProperty::begin_object()
{
    return ObjectIterator(this, 0);
}

BindingProperty::ObjectIterator BindingProperty::end_object()
{
    switch (flags)
    {
    case kObjectFlag:
        return ObjectIterator(this, data.objectVal.size());
    default:
        return ObjectIterator(this, 0);
    }
}

BindingProperty::ConstObjectIterator BindingProperty::begin_object() const
{
    return ConstObjectIterator(this, 0);
}

BindingProperty::ConstObjectIterator BindingProperty::end_object() const
{
    switch (flags)
    {
    case kObjectFlag:
        return ConstObjectIterator(this, data.objectVal.size());
    default:
        return ConstObjectIterator(this, 0);
    }
}

}; // namespace CubeWorld
