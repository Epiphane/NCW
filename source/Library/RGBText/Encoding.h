// By Thomas Steinke

#pragma once

#include <locale>
#include <codecvt>
#include <string>

namespace CubeWorld
{

// Converts to and from wide strings (wchar_t)
class WideConverter
{
public:
   std::wstring Utf8ToWide(const std::string& utf8);
   std::string WideToUtf8(const std::wstring& wide);

private:
   std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
};

//
// Helpers to skip constructing a WideConverter
//
inline std::wstring Utf8ToWide(const std::string& utf8) { return WideConverter{}.Utf8ToWide(utf8); }
inline std::string WideToUtf8(const std::wstring& wide) { return WideConverter{}.WideToUtf8(wide); }

}; // namespace CubeWorld
