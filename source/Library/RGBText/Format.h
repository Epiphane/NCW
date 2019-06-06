// By Thomas Steinke

#pragma once

#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

namespace CubeWorld
{

namespace Format
{

namespace impl
{

enum type
{
   none_type, name_arg_type,
   // Integer types should go first,
   int32_type, uint32_type, int64_type, uint64_type, bool_type, char_type,
   last_integer_type = char_type,
   // followed by floating-point types.
   double_type, long_double_type, last_numeric_type = long_double_type,
   cstring_type, string_type, pointer_type, custom_type
};

constexpr bool is_integral(type t) {
   assert(t != type::name_arg_type);
   return t > type::none_type && t <= type::last_integer_type;
}

constexpr bool is_arithmetic(type t) {
   assert(t != type::name_arg_type);
   return t > type::none_type && t <= type::last_numeric_type;
}

template <typename Char>
struct string_value
{
   const Char *value;
   std::size_t size;
};

struct custom_value {
   const void *value;
   void(*format)(const void *arg);
};

class value
{
public:
   union {
      int32_t int32_value;
      int64_t int64_value;
      uint32_t uint32_value;
      uint64_t uint64_value;
      double double_value;
      long double long_double_value;
      const void *pointer;
      string_value<char> string;
      string_value<signed char> sstring;
      string_value<unsigned char> ustring;
      custom_value custom;
   };

   // constexpr the first one, but the rest can't be because union?
   constexpr value(int32_t val = 0) : int32_value(val) {}
   value(int64_t val) { int64_value = val; }
   value(uint32_t val) { uint32_value = val; }
   value(uint64_t val) { uint64_value = val; }
   value(double val) { double_value = val; }
   value(long double val) { long_double_value = val; }
   value(const void *val) { pointer = val; }
   value(const signed char *val) { sstring.value = val; }
   value(const unsigned char *val) { ustring.value = val; }
   value(std::basic_string_view<char> val)
   {
      string.value = val.data();
      string.size = val.size();
   }

   /*template <typename T>
   explicit value(const T &val) {
   custom.value = &val;
   custom.format = &format_custom_arg<T>;
   }*/

   // TODO named_arg_base as_named_arg

private:
   // Formats a custom argument, such as a user-defined class.
   template <typename T>
   static void format_custom_arg(const void * /* arg */)
   {
      // TODO
      /*static_assert(false, "Unimplemented");*/
   }
};

template <type TYPE>
struct typed_value : value {
   static const type type_tag = TYPE;

   template <typename T>
   constexpr typed_value(const T &val) : value(val) {}
};

struct format_specs
{
   uint32_t precision = UINT32_MAX;
};

class basic_arg
{
private:
   value value_;
   type type_;

   template <typename T>
   friend constexpr basic_arg make_arg(const T &value);

   friend std::string FormatArg(const basic_arg& argument, const format_specs& precision);
   friend class basic_format_args;

public:
   // TODO
   /*
   class handle
   {
   public:
      explicit handle(custom_value custom) : custom_(custom) {}

      void format()
      {
         // TODO
         assert(false);
      }

   private:
      // TODO
      custom_value custom;
   };*/

   constexpr basic_arg() : type_(type::none_type) {}

   explicit operator bool() const noexcept
   {
      return type_ != type::none_type;
   }

   type type() const { return type_; }

   bool is_integral() const { return impl::is_integral(type_); }
   bool is_arithmetic() const { return impl::is_arithmetic(type_); }
   bool is_pointer() const { return type_ == type::pointer_type; }
};

#define MAKE_VALUE(TAG, ArgType, ValueType) constexpr \
   typed_value<TAG> make_value(ArgType val) \
   { \
      return static_cast<ValueType>(val); \
   }

MAKE_VALUE(type::int32_type,   signed char,         int32_t);
MAKE_VALUE(type::int32_type,   short,               int32_t);
MAKE_VALUE(type::int32_type,   int,                 int32_t);
MAKE_VALUE(type::uint32_type,  unsigned char,      uint32_t);
MAKE_VALUE(type::uint32_type,  unsigned short,     uint32_t);
MAKE_VALUE(type::uint32_type,  unsigned int,       uint32_t);
MAKE_VALUE(type::int64_type,   long long,           int64_t);
MAKE_VALUE(type::uint64_type,  unsigned long long, uint64_t);
MAKE_VALUE(type::bool_type,    bool,                int32_t);
MAKE_VALUE(type::char_type,    char,                int32_t);

// To minimize the number of types we need to deal with, long is translated
// either to int or to long long depending on its size.
typedef std::conditional<sizeof(long) == sizeof(int), int, long long>::type long_type;
MAKE_VALUE((sizeof(long) == sizeof(int) ? type::int32_type : type::int64_type), long, long_type);
typedef std::conditional<sizeof(unsigned long) == sizeof(unsigned int), unsigned int, unsigned long long>::type ulong_type;
MAKE_VALUE((sizeof(unsigned long) == sizeof(unsigned int) ? type::uint32_type : type::uint64_type), unsigned long, ulong_type);

MAKE_VALUE(type::double_type, float,  double);
MAKE_VALUE(type::double_type, double, double);
MAKE_VALUE(type::long_double_type, long double, long double);

MAKE_VALUE(type::cstring_type, char*,                const char*);
MAKE_VALUE(type::cstring_type, const char*,          const char*);
MAKE_VALUE(type::cstring_type, signed char*,         const signed char*);
MAKE_VALUE(type::cstring_type, const signed char*,   const signed char*);
MAKE_VALUE(type::cstring_type, unsigned char*,       const unsigned char*);
MAKE_VALUE(type::cstring_type, const unsigned char*, const unsigned char*);
MAKE_VALUE(type::string_type, std::basic_string_view<char>, std::basic_string_view<char>);
MAKE_VALUE(type::string_type, const std::basic_string<char>&, std::basic_string_view<char>);

MAKE_VALUE(type::pointer_type, void*, const void*);
MAKE_VALUE(type::pointer_type, const void*, const void*);
MAKE_VALUE(type::pointer_type, std::nullptr_t, const void*);

// Formatting of arbitrary pointers is disallowed. If you want to output a
// pointer cast it to "void *" or "const void *". In particular, this forbids
// formatting of "[const] volatile char *" which is printed as bool by
// iostreams.
template <typename T>
void make_value(const T *) {
   static_assert(!sizeof(T), "formatting of non-void pointers is disallowed");
}

enum { MAX_PACKED_ARGS = 15 };

template <typename T>
class get_type
{
public:
   typedef decltype(make_value(
      std::declval<typename std::decay<T>::type>())) value_type;

   static const type value = value_type::type_tag;
};
   
template <typename T>
constexpr basic_arg make_arg(const T &value)
{
   basic_arg arg;
   arg.type_ = get_type<T>::value;
   arg.value_ = make_value(value);
   return arg;
}

template <typename ...Args>
class arg_store;

class basic_format_args
{
public:
   typedef unsigned size_type;

private:
   size_t size_;
   const basic_arg *args_;

   void set_data(const basic_arg *args) { args_ = args; }

   basic_arg get(uint32_t index) const
   {
      return index < size_ ? args_[index] : basic_arg{};
   }

public:
   basic_format_args() : size_(0), args_(nullptr) {}

   template<typename... Args>
   basic_format_args(const arg_store<Args...> &store) : size_(store.NUM_ARGS), args_(store.data()) {}

   basic_arg operator[](uint32_t index) const
   {
      basic_arg arg = get(index);
      return arg;
   }

   size_t max_size() const {
      return size_;
   }
};

template <typename ...Args>
class arg_store {
public:
   static const size_t NUM_ARGS = sizeof...(Args);
   static const uint64_t TYPES;

private:
   // Packed is a macro on MinGW so use IS_PACKED instead.
   // TODO packed?
   static const bool IS_PACKED = NUM_ARGS < MAX_PACKED_ARGS;

   //typedef typename std::conditional<IS_PACKED, value, basic_arg> value_type;
   typedef class basic_arg value_type;

   // If the arguments are not packed, add one more element to mark the end.
   basic_arg data_[NUM_ARGS + 1];
   // (IS_PACKED && NUM_ARGS != 0 ? 0 : 1)];

public:
   arg_store(const Args&... args) : data_{ impl::make_arg(args)... } {}

   basic_format_args operator*() const { return *this; }

   const value_type *data() const { return data_; }
};

// TODO used for packing
template <typename ...Args>
const uint64_t arg_store<Args...>::TYPES = -static_cast<int64_t>(NUM_ARGS);

template <typename ...Args>
inline arg_store<Args...> make_args(const Args & ... args)
{
   return arg_store<Args...>(args...);
}

}; // namespace impl

std::string FormatString(std::string_view fmt, impl::basic_format_args args);

// 
// Custom string formatter, which allows for type-agnostic insertion of arguments.
//
// Example: FormatString("This incorporates %1 values, starting with %2 and %3", 2, "my string", &myObject);
//
// Usual types from printf are also "supported" (incomplete), such as %d, %s, etc.
//
template <typename... Args>
std::string FormatString(std::string_view fmt, const Args& ... args)
{
   return FormatString(fmt, *impl::arg_store<Args...>(args...));
}

}; // namespace Input

}; // namespace CubeWorld
