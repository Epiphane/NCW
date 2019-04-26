// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved. See License.txt in the project root for license information.

#pragma once

#if !defined(RXCPP_RX_OPERATORS_HPP)
#define RXCPP_RX_OPERATORS_HPP

#include "rx-includes.h"

namespace rxcpp {

namespace operators {

struct tag_operator {};
template<class T>
struct operator_base
{
    typedef T value_type;
    typedef tag_operator operator_tag;
};

namespace detail {

template<class T, class =rxu::types_checked>
struct is_operator : std::false_type
{
};

template<class T>
struct is_operator<T, rxu::types_checked_t<typename T::operator_tag>> 
    : std::is_convertible<typename T::operator_tag*, tag_operator*>
{
};

}

template<class T, class Decayed = rxu::decay_t<T>>
struct is_operator : detail::is_operator<Decayed>
{
};


}
namespace rxo=operators;

template<class Tag> 
struct member_overload
{
    template<class... AN>
    static auto member(AN&&...) ->
                typename Tag::template include_header<std::false_type> {
        return  typename Tag::template include_header<std::false_type>();
    }
};

template<class T, class... AN>
struct delayed_type{using value_type = T; static T value(AN**...) {return T{};}};

template<class T, class... AN>
using delayed_type_t = rxu::value_type_t<delayed_type<T, AN...>>;

template<class Tag, class... AN, class Overload = member_overload<rxu::decay_t<Tag>>>
auto observable_member(Tag, AN&&... an) -> 
    decltype(Overload::member(std::forward<AN>(an)...)) {
    return   Overload::member(std::forward<AN>(an)...);
}

template<class Tag, class... AN>
class operator_factory
{
    using this_type = operator_factory<Tag, AN...>;
    using tag_type = rxu::decay_t<Tag>;
    using tuple_type = std::tuple<rxu::decay_t<AN>...>;
    
    tuple_type an;

public:
    operator_factory(tuple_type an)
        : an(std::move(an))
    {
    }

    template<class... ZN>
    auto operator()(tag_type t, ZN&&... zn) const
        -> decltype(observable_member(t, std::forward<ZN>(zn)...)) {
        return      observable_member(t, std::forward<ZN>(zn)...);
    }

    template<class Observable>
    auto operator()(Observable source) const 
        -> decltype(rxu::apply(std::tuple_cat(std::make_tuple(tag_type{}, source), (*(tuple_type*)nullptr)), (*(this_type*)nullptr))) {
        return      rxu::apply(std::tuple_cat(std::make_tuple(tag_type{}, source),                      an),                  *this);
    }
};

}

#include "operators/rx-lift.h"
#include "operators/rx-subscribe.h"

namespace rxcpp {

struct amb_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-amb.h>");
    };
};

struct all_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-all.h>");
    };
};

struct is_empty_tag : all_tag {};

struct any_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-any.h>");
    };
};

struct exists_tag : any_tag {};
struct contains_tag : any_tag {};

struct buffer_count_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-buffer_count.h>");
    };
};

struct buffer_with_time_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-buffer_time.h>");
    };
};

struct buffer_with_time_or_count_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-buffer_time_count.h>");
    };
};

struct combine_latest_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-combine_latest.h>");
    };
};

struct concat_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-concat.h>");
    };
};

struct concat_map_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-concat_map.h>");
    };
};

struct connect_forever_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-connect_forever.h>");
    };
};

struct debounce_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-debounce.h>");
    };
};

struct delay_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-delay.h>");
    };
};

struct distinct_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-distinct.h>");
    };
};

struct distinct_until_changed_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-distinct_until_changed.h>");
    };
};

struct element_at_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-element_at.h>");
    };
};

struct filter_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-filter.h>");
    };
};

struct finally_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-finally.h>");
    };
};

struct flat_map_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-flat_map.h>");
    };
};

struct group_by_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-group_by.h>");
    };
};

struct ignore_elements_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-ignore_elements.h>");
    };
};

struct map_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-map.h>");
    };
};

struct merge_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-merge.h>");
    };
};
struct merge_delay_error_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-merge_delay_error.h>");
    };
};

struct multicast_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-multicast.h>");
    };
};

struct observe_on_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-observe_on.h>");
    };
};

struct on_error_resume_next_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-on_error_resume_next.h>");
    };
};

class empty_error: public std::runtime_error
{
    public:
        explicit empty_error(const std::string& msg):
            std::runtime_error(msg)
        {}
};
struct reduce_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-reduce.h>");
    };
};
struct first_tag : reduce_tag {};
struct last_tag : reduce_tag {};
struct sum_tag : reduce_tag {};
struct average_tag : reduce_tag {};
struct min_tag : reduce_tag {};
struct max_tag : reduce_tag {};

struct ref_count_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-ref_count.h>");
    };
};

struct pairwise_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-pairwise.h>");
    };
};

struct publish_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-publish.h>");
    };
};
struct publish_synchronized_tag : publish_tag {};
    
struct repeat_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-repeat.h>");
    };
};

struct replay_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-replay.h>");
    };
};

struct retry_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-retry.h>");
    };
};

struct sample_with_time_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-sample_time.h>");
    };
};

struct scan_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-scan.h>");
    };
};

struct sequence_equal_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-sequence_equal.h>");
    };
};

struct skip_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-skip.h>");
    };
};

struct skip_while_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-skip_while.h>");
    };
};

struct skip_last_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-skip_last.h>");
    };
};

struct skip_until_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-skip_until.h>");
    };
};

struct start_with_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-start_with.h>");
    };
};

struct subscribe_on_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-subscribe_on.h>");
    };
};

struct switch_if_empty_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-switch_if_empty.h>");
    };
};
struct default_if_empty_tag : switch_if_empty_tag {};

struct switch_on_next_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-switch_on_next.h>");
    };
};

struct take_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-take.h>");
    };
};

struct take_last_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-take_last.h>");
    };
};

struct take_while_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-take_while.h>");
    };
};

struct take_until_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-take_until.h>");
    };
};

struct tap_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-rap.h>");
    };
};

struct timeout_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-timeout.h>");
    };
};

struct time_interval_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-time_interval.h>");
    };
};

struct timestamp_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-timestamp.h>");
    };
};

struct window_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-window.h>");
    };
};

struct window_with_time_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-window_time.h>");
    };
};

struct window_with_time_or_count_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-window_time_count.h>");
    };
};

struct window_toggle_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-window_toggle.h>");
    };
};

struct with_latest_from_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-with_latest_from.h>");
    };
};

struct zip_tag {
    template<class Included>
    struct include_header{
        static_assert(Included::value, "missing include: please #include <rxcpp/operators/rx-zip.h>");
    };
};

}

#include "operators/rx-multicast.h"
#include "operators/rx-publish.h"
#include "operators/rx-ref_count.h"

#endif
