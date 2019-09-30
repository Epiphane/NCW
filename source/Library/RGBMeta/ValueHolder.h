/* -----------------------------------------------------------------------------------------------

ValueHolder holds all Member objects constructed via meta::registerValues<T> call.
If the class is not registered, values is std::tuple<>

-------------------------------------------------------------------------------------------------*/

#pragma once

#include <tuple>

namespace CubeWorld
{

namespace Meta
{

template <typename T, typename TupleType>
struct ValueHolder {
    static TupleType values;
    static const char* name()
    {
        return registerName<T>();
    }
};

template <typename T, typename TupleType>
TupleType ValueHolder<T, TupleType>::values = meta::registerValues<T>();


}; // namespace Meta

}; // namespace CubeWorld
