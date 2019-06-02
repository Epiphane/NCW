// By Thomas Steinke

#include <cmath>
#include <cstdarg>
#include <stdio.h>
#include <string_view>
#include <type_traits>
#include <utility>

#include "Format.h"

namespace CubeWorld
{

namespace Format
{

const char DIGITS[] =
"0001020304050607080910111213141516171819"
"2021222324252627282930313233343536373839"
"4041424344454647484950515253545556575859"
"6061626364656667686970717273747576777879"
"8081828384858687888990919293949596979899";

// Inspired by fmt::FormatInt
class FormatInt
{
private:
   const static uint16_t BUFFER_SIZE = std::numeric_limits<unsigned long long>::digits10 + 3;

   char buffer[BUFFER_SIZE];
   char *str;

   char *format_decimal(uint64_t value)
   {
      // Point to the end
      char *ptr = buffer + BUFFER_SIZE - 1;
      while (value >= 100)
      {
         // Integer division is slow so do it for a group of two digits instead
         // of for every digit. The idea comes from the talk by Alexandrescu
         // "Three Optimization Tips for C++". See speed-test for a comparison.
         unsigned index = static_cast<unsigned>((value % 100) * 2);
         value /= 100;
         *--ptr = DIGITS[index + 1];
         *--ptr = DIGITS[index];
      }
      if (value < 10)
      {
         *--ptr = static_cast<char>('0' + value);
         return ptr;
      }
      unsigned index = static_cast<unsigned>(value * 2);
      *--ptr = DIGITS[index + 1];
      *--ptr = DIGITS[index];
      return ptr;
   }

   void format_signed(int64_t value)
   {
      uint64_t abs_value = static_cast<uint64_t>(value);
      bool negative = value < 0;
      if (negative) { abs_value = 0 - abs_value; }
      str = format_decimal(abs_value);
      if (negative) { *--str = '-'; }
   }

public:
   explicit FormatInt(int16_t value) { format_signed(value); }
   explicit FormatInt(int32_t value) { format_signed(value); }
   explicit FormatInt(int64_t value) { format_signed(value); }
   explicit FormatInt(uint16_t value) : str(format_decimal(value)) {}
   explicit FormatInt(uint32_t value) : str(format_decimal(value)) {}
   explicit FormatInt(uint64_t value) : str(format_decimal(value)) {}

   std::string result() const { return std::string(str, uint64_t(buffer - str + BUFFER_SIZE - 1)); }
};

std::string FormatBool(bool value)
{
   return value ? "true" : "false";
}

std::string FormatChar(char value)
{
   return std::string(1, value);
}

// Overload used so that we only append L on a long double
template <typename T>
void AppendFloatLength(char *&, T) {}
void AppendFloatLength(char *&p, long double) { *p++ = 'L'; }

template <typename T>
std::string FormatDouble(T value, const impl::format_specs& specs)
{
   switch (fpclassify(value)) {
   case FP_INFINITE:  return "<inf>";
   case FP_NAN:       return "<nan>";
   case FP_ZERO:      return "0";
   case FP_SUBNORMAL: return "<err>";
   }

   // TODO I'm lazy..
   static const size_t BUFFER_SIZE = std::numeric_limits<T>::digits10 + 1;
   static const size_t MAX_FORMAT_SIZE = 10; // longest format: %#-*.*Lg
   char buffer[BUFFER_SIZE];
   char fmt[MAX_FORMAT_SIZE];
   char *pFmt = fmt;
   *pFmt++ = '%';
   // TODO #-*.* flags
   if (specs.precision != UINT32_MAX)
   {
      assert(specs.precision < 10);
      *pFmt++ = '.';
      *pFmt++ = ('0' + static_cast<char>(specs.precision));
   }
   AppendFloatLength(pFmt, value);
   *pFmt++ = 'f'; // TODO more things
   *pFmt = '\0';

   // TODO snprintf_s or _snprintf_s by platform
   int written = snprintf(buffer, BUFFER_SIZE, fmt, value);
   if (written <= (int)BUFFER_SIZE)
   {
      return std::string(buffer);
   }
   return "<error>";
}

std::string FormatPointer(const void *pointer)
{
   return "<pointer:" + FormatInt(reinterpret_cast<uint64_t>(pointer)).result() + ">";
}

namespace impl {
std::string FormatArg(const impl::basic_arg& argument, const format_specs& specs)
{
   switch (argument.type_)
   {
   case impl::type::none_type:
   case impl::type::name_arg_type:
      assert(false);
   case impl::type::int32_type:
      return FormatInt(argument.value_.int32_value).result();
   case impl::type::uint32_type:
      return FormatInt(argument.value_.uint32_value).result();
   case impl::type::int64_type:
      return FormatInt(argument.value_.int64_value).result();
   case impl::type::uint64_type:
      return FormatInt(argument.value_.uint64_value).result();
   case impl::type::bool_type:
      return FormatBool(argument.value_.int32_value != 0);
   case impl::type::char_type:
      return FormatChar(static_cast<char>(argument.value_.int32_value));
   case impl::type::double_type:
      return FormatDouble(argument.value_.double_value, specs);
   case impl::type::long_double_type:
      return FormatDouble(argument.value_.long_double_value, specs);
   case impl::type::cstring_type:
      return std::string(argument.value_.string.value);
   case impl::type::string_type:
      return std::string(argument.value_.string.value, argument.value_.string.size);
   case impl::type::pointer_type:
      return FormatPointer(argument.value_.pointer);
   case impl::type::custom_type:
      break;
   }

   return "<unimplemented>";
}
}; // namespace impl

template <typename... Args>
size_t BufferSize(std::string_view string, impl::basic_format_args args)
{
   return string.length() + 16 * args.max_size();
}

// Parse a nonnegative integer out of a string.
// NO ERROR HANDLING. If a number is too big, rest in peace my dude.
uint32_t ParseNonnegativeInteger(std::string_view::iterator& it, std::string_view::iterator end)
{
   assert('0' <= *it && *it <= '9');
   uint32_t value = 0;

   uint32_t max_int = (std::numeric_limits<uint32_t>::max)();
   uint32_t big = max_int / 10;
   do
   {
      // Overflow check.
      if (value > big)
      {
         value = max_int + 1;
         break;
      }

      value = value * 10 + (*it++ - '0');
   } while (it != end && '0' <= *it && *it <= '9');
   return value;
}

std::string FormatString(std::string_view fmt, impl::basic_format_args args)
{
   std::string result;
   result.reserve(BufferSize(fmt, args));

   // Current argument, for non-positional formatted strings.
   uint32_t _arg = 0;

   std::string_view::iterator it = fmt.begin();
   std::string_view::iterator end = fmt.end();
   std::string_view::iterator start = it;
   while (it != end)
   {
      char ch = *it++;
      if (ch != '%') continue;
      if (*it == ch) {
         // Append simple text.
         result.append(start, it);
         start = ++it;
         continue;
      }

      // Append everything before %
      result.append(start, it - 1);

      // Parse argument index, flags, and width.
      uint32_t arg_index = std::numeric_limits<uint32_t>::max();

      char c = *it;
      if (c >= '0' && c <= '9')
      {
         // Parse argument index (followed by '$') or a width preceded with '0'.
         arg_index = ParseNonnegativeInteger(it, end) - 1;
      }

      if (arg_index == std::numeric_limits<uint32_t>::max())
      {
         arg_index = _arg++;
      }

      impl::format_specs specs;
      if (it != end)
      {
         // Parse format
         // TODO
         if (*it == '.')
         {
            ++it;
            if ('0' <= *it && *it <= '9')
            {
               specs.precision = ParseNonnegativeInteger(it, end);
            }

            // TODO there's a lot more to worry about here i think
            assert(*it++ == 'f' && "Format type of f is the only one implemented with precision");
         }
      }

      impl::basic_arg arg = args[arg_index];

      result.append(FormatArg(arg, specs));

      start = it;
   }
   result.append(start, it);

   return result;
}

}; // namespace Format

}; // namespace CubeWorld
