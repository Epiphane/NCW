// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved. See License.txt in the project root for license information.

#pragma once

#if !defined(RXCPP_RX_INCLUDES_HPP)
#define RXCPP_RX_INCLUDES_HPP

#include "rx-trace.h"

// some configuration macros
#if defined(_MSC_VER)

#if _MSC_VER > 1600
#pragma warning(disable: 4348) // false positives on : redefinition of default parameter : parameter 2
#define RXCPP_USE_RVALUEREF 1
#endif

#if _MSC_VER >= 1800
#define RXCPP_USE_VARIADIC_TEMPLATES 1
#endif

#if _CPPRTTI
#define RXCPP_USE_RTTI 1
#endif

#if _HAS_EXCEPTIONS
#define RXCPP_USE_EXCEPTIONS 1
#endif

#define RXCPP_NORETURN __declspec(noreturn)

#elif defined(__clang__)

#if __has_feature(cxx_rvalue_references)
#define RXCPP_USE_RVALUEREF 1
#endif
#if __has_feature(cxx_rtti)
#define RXCPP_USE_RTTI 1
#endif
#if __has_feature(cxx_variadic_templates)
#define RXCPP_USE_VARIADIC_TEMPLATES 1
#endif
#if __has_feature(cxx_exceptions)
#define RXCPP_USE_EXCEPTIONS 1
#endif

#if __has_feature(cxx_attributes)
#define RXCPP_NORETURN [[noreturn]]
#else
#define RXCPP_NORETURN __attribute__ ((noreturn))
#endif

#elif defined(__GNUG__)

#define GCC_VERSION (__GNUC__ * 10000 + \
                     __GNUC_MINOR__ * 100 + \
                     __GNUC_PATCHLEVEL__)

#if GCC_VERSION >= 40801
#define RXCPP_USE_RVALUEREF 1
#endif

#if GCC_VERSION >= 40400
#define RXCPP_USE_VARIADIC_TEMPLATES 1
#endif

#if defined(__GXX_RTTI)
#define RXCPP_USE_RTTI 1
#endif

#if defined(__EXCEPTIONS)
#define RXCPP_USE_EXCEPTIONS 1
#endif

#define RXCPP_NORETURN __attribute__ ((noreturn))

#endif

//
// control std::hash<> of enum
// force with RXCPP_FORCE_HASH_ENUM & RXCPP_FORCE_HASH_ENUM_UNDERLYING
// in time use ifdef to detect library support for std::hash<> of enum
//
#define RXCPP_HASH_ENUM 0
#define RXCPP_HASH_ENUM_UNDERLYING 1

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#define RXCPP_USE_WINRT 0
#else
#define RXCPP_USE_WINRT 1
#endif

#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if (TARGET_OS_IPHONE == 1) || (TARGET_IPHONE_SIMULATOR == 1)
#define RXCPP_ON_IOS
#endif
#endif

#if defined(__ANDROID__)
#define RXCPP_ON_ANDROID
#endif

#if defined(RXCPP_FORCE_USE_VARIADIC_TEMPLATES)
#undef RXCPP_USE_VARIADIC_TEMPLATES
#define RXCPP_USE_VARIADIC_TEMPLATES RXCPP_FORCE_USE_VARIADIC_TEMPLATES
#endif

#if defined(RXCPP_FORCE_USE_RVALUEREF)
#undef RXCPP_USE_RVALUEREF
#define RXCPP_USE_RVALUEREF RXCPP_FORCE_USE_RVALUEREF
#endif

#if defined(RXCPP_FORCE_USE_RTTI)
#undef RXCPP_USE_RTTI
#define RXCPP_USE_RTTI RXCPP_FORCE_USE_RTTI
#endif

#if defined(RXCPP_FORCE_USE_EXCEPTIONS)
#undef RXCPP_USE_EXCEPTIONS
#define RXCPP_USE_EXCEPTIONS RXCPP_FORCE_USE_EXCEPTIONS
#endif

#if defined(RXCPP_FORCE_USE_WINRT)
#undef RXCPP_USE_WINRT
#define RXCPP_USE_WINRT RXCPP_FORCE_USE_WINRT
#endif

#if defined(RXCPP_FORCE_HASH_ENUM)
#undef RXCPP_HASH_ENUM
#define RXCPP_HASH_ENUM RXCPP_FORCE_HASH_ENUM
#endif

#if defined(RXCPP_FORCE_HASH_ENUM_UNDERLYING)
#undef RXCPP_HASH_ENUM_UNDERLYING
#define RXCPP_HASH_ENUM_UNDERLYING RXCPP_FORCE_HASH_ENUM_UNDERLYING
#endif

#if defined(RXCPP_FORCE_ON_IOS)
#undef RXCPP_ON_IOS
#define RXCPP_ON_IOS RXCPP_FORCE_ON_IOS
#endif

#if defined(RXCPP_FORCE_ON_ANDROID)
#undef RXCPP_ON_ANDROID
#define RXCPP_ON_ANDROID RXCPP_FORCE_ON_ANDROID
#endif

#if defined(_MSC_VER) && !RXCPP_USE_VARIADIC_TEMPLATES
// resolve args needs enough to store all the possible resolved args
#define _VARIADIC_MAX 10
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1800)
#define RXCPP_NOEXCEPT
#else
#define RXCPP_NOEXCEPT noexcept
#endif

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

#include <stdlib.h>

#include <cstddef>

#include <iostream>
#include <iomanip>

#include <exception>
#include <functional>
#include <memory>
#include <array>
#include <vector>
#include <algorithm>
#include <atomic>
#include <map>
#include <set>
#include <mutex>
#include <deque>
#include <thread>
#include <future>
#include <list>
#include <queue>
#include <chrono>
#include <condition_variable>
#include <initializer_list>
#include <typeinfo>
#include <tuple>
#include <unordered_set>
#include <type_traits>
#include <utility>

#if defined(RXCPP_ON_IOS) || defined(RXCPP_ON_ANDROID)
#include <pthread.h>
#endif

#include "rx-util.h"
#include "rx-predef.h"
#include "rx-subscription.h"
#include "rx-observer.h"
#include "rx-scheduler.h"
#include "rx-subscriber.h"
#include "rx-notification.h"
#include "rx-coordination.h"
#include "rx-sources.h"
#include "rx-subjects.h"
#include "rx-operators.h"
#include "rx-observable.h"
#include "rx-connectable_observable.h"
#include "rx-grouped_observable.h"

#if !defined(RXCPP_LITE)
#include "operators/rx-all.h"
#include "operators/rx-amb.h"
#include "operators/rx-any.h"
#include "operators/rx-buffer_count.h"
#include "operators/rx-buffer_time.h"
#include "operators/rx-buffer_time_count.h"
#include "operators/rx-combine_latest.h"
#include "operators/rx-concat.h"
#include "operators/rx-concat_map.h"
#include "operators/rx-connect_forever.h"
#include "operators/rx-debounce.h"
#include "operators/rx-delay.h"
#include "operators/rx-distinct.h"
#include "operators/rx-distinct_until_changed.h"
#include "operators/rx-element_at.h"
#include "operators/rx-filter.h"
#include "operators/rx-finally.h"
#include "operators/rx-flat_map.h"
#include "operators/rx-group_by.h"
#include "operators/rx-ignore_elements.h"
#include "operators/rx-map.h"
#include "operators/rx-merge.h"
#include "operators/rx-merge_delay_error.h"
#include "operators/rx-observe_on.h"
#include "operators/rx-on_error_resume_next.h"
#include "operators/rx-pairwise.h"
#include "operators/rx-reduce.h"
#include "operators/rx-repeat.h"
#include "operators/rx-replay.h"
#include "operators/rx-retry.h"
#include "operators/rx-sample_time.h"
#include "operators/rx-scan.h"
#include "operators/rx-sequence_equal.h"
#include "operators/rx-skip.h"
#include "operators/rx-skip_while.h"
#include "operators/rx-skip_last.h"
#include "operators/rx-skip_until.h"
#include "operators/rx-start_with.h"
#include "operators/rx-subscribe_on.h"
#include "operators/rx-switch_if_empty.h"
#include "operators/rx-switch_on_next.h"
#include "operators/rx-take.h"
#include "operators/rx-take_last.h"
#include "operators/rx-take_until.h"
#include "operators/rx-take_while.h"
#include "operators/rx-tap.h"
#include "operators/rx-time_interval.h"
#include "operators/rx-timeout.h"
#include "operators/rx-timestamp.h"
#include "operators/rx-window.h"
#include "operators/rx-window_time.h"
#include "operators/rx-window_time_count.h"
#include "operators/rx-window_toggle.h"
#include "operators/rx-with_latest_from.h"
#include "operators/rx-zip.h"
#endif

#pragma pop_macro("min")
#pragma pop_macro("max")

#endif
