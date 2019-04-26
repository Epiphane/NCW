
// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved. See License.txt in the project root for license information.

#pragma once

#if !defined(RXCPP_RX_SOURCES_HPP)
#define RXCPP_RX_SOURCES_HPP

#include "rx-includes.h"

namespace rxcpp {

namespace sources {

struct tag_source {};
template<class T>
struct source_base
{
    typedef T value_type;
    typedef tag_source source_tag;
};
template<class T>
class is_source
{
    template<class C>
    static typename C::source_tag* check(int);
    template<class C>
    static void check(...);
public:
    static const bool value = std::is_convertible<decltype(check<rxu::decay_t<T>>(0)), tag_source*>::value;
};

}
namespace rxs=sources;

}

#include "sources/rx-create.h"
#include "sources/rx-range.h"
#include "sources/rx-iterate.h"
#include "sources/rx-interval.h"
#include "sources/rx-empty.h"
#include "sources/rx-defer.h"
#include "sources/rx-never.h"
#include "sources/rx-error.h"
#include "sources/rx-scope.h"
#include "sources/rx-timer.h"

#endif
