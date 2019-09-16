// By Thomas Steinke

#include "../../catch.h"

#include <RGBText/Format.h>

namespace CubeWorld
{

TEST_CASE("FormatString with positional arguments") {
   CHECK(
      FormatString("Test string with %1 arguments, %2 and %1", 2, "argument 1")
      == "Test string with 2 arguments, argument 1 and 2"
   );
}

TEST_CASE("FormatString with special characters") {
   CHECK(
      FormatString("Test string using \\%1 and \\{arg}")
      == "Test string using \\%1 and \\{arg}"
   );
}

TEST_CASE("FormatString with named arguments") {
   CHECK(
      FormatString("Test string with {num} arguments, {arg1} and {arg2}", 2, "argument 1", 2.0)
      == "Test string with 2 arguments, argument 1 and 2.000000"
   );
}

TEST_CASE("FormatString types") {
   CHECK(FormatString("Int: {int}", 2) == "Int: 2");
   CHECK(FormatString("Char: {char}", 'c') == "Char: c");
   CHECK(FormatString("Float: {float}", 2.0f) == "Float: 2.000000");
   CHECK(FormatString("Double: {double}", -2.0) == "Double: -2.000000");
   CHECK(FormatString("String: {string}", "char *") == "String: char *");
   CHECK(FormatString("String: {string}", std::string("str")) == "String: str");
   CHECK(FormatString("Pointer: {ptr}", nullptr) == "Pointer: <pointer:0>");
}

TEST_CASE("FormatString errors") {
   CHECK(FormatString("No arg: {}") == "No arg: <missing arg>");
   CHECK(FormatString("Unclosed brace: {whatever man...") == "Unclosed brace: <missing arg>");
}

}; // namespace CubeWorld
