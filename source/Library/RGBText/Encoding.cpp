// By Thomas Steinke

#pragma once

#include "Encoding.h"

namespace CubeWorld
{

std::wstring WideConverter::Utf8ToWide(const std::string& utf8)
{
   return converter.from_bytes(utf8);
}

std::string WideConverter::WideToUtf8(const std::wstring& wide)
{
   return converter.to_bytes(wide);
}

}; // namespace CubeWorld
