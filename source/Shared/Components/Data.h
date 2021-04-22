// By Thomas Steinke

#pragma once

#include <unordered_map>

#include <RGBBinding/BindingProperty.h>
#include <Engine/Entity/Component.h>

namespace CubeWorld
{

//
// A Data component can hold any assortment of random data key/value pairs.
//
struct Data : public Engine::Component<Data> {
    BindingProperty& Generic(const std::string& key)
    {
        return mData[key];
    }

    float& Float(const std::string& key)
    {
        return mFloats[key];
    }

private:
    std::unordered_map<std::string, BindingProperty> mData;
    std::unordered_map<std::string, float> mFloats;
};

}; // namespace CubeWorld
