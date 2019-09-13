// By Thomas Steinke

#include <algorithm>
#include <string>
#include <vector>

#include <Windows.h>
#include <Commdlg.h>

#include <RGBText/Encoding.h>
#include "../Paths.h"
#include "../File.h"

namespace CubeWorld
{

std::string OpenFileDialog(
   const std::string& defaultFile,
   const std::vector<std::string>& /*fileTypes*/
)
{
   // Inspiration: https://docs.microsoft.com/en-us/windows/desktop/dlgbox/using-common-dialog-boxes
   OPENFILENAMEW ofn;    // common dialog box structure
   wchar_t szFile[260];    // buffer for file name
   HWND hwnd = nullptr; // owner window

   std::string canonical = Paths::Canonicalize(defaultFile);
   std::replace(canonical.begin(), canonical.end(), '/', '\\');

   wcsncpy_s(szFile, Utf8ToWide(canonical).c_str(), sizeof(szFile));

   // Initialize ofn options
   ZeroMemory(&ofn, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFile = szFile;
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFilter = L"YAML\0*.yaml\0JSON\0*.json\0All\0*.*\0";
   ofn.nFilterIndex = 1;
   ofn.lpstrFileTitle = NULL;
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = NULL; // Inherits from lpstrFile
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

   // Display the Open dialog box.
   if (GetOpenFileNameW(&ofn) == TRUE)
   {
#pragma warning(disable : 6054)
      return WideToUtf8(ofn.lpstrFile);
#pragma warning(default : 6054)
   }

   return "";
}

std::string SaveFileDialog(
   const std::string& defaultFile
)
{
   // Inspiration: https://docs.microsoft.com/en-us/windows/desktop/dlgbox/using-common-dialog-boxes
   OPENFILENAMEW ofn;    // common dialog box structure
   wchar_t szFile[260];    // buffer for file name
   HWND hwnd = nullptr; // owner window

   std::string canonical = Paths::Canonicalize(defaultFile);
   std::replace(canonical.begin(), canonical.end(), '/', '\\');

   wcsncpy_s(szFile, Utf8ToWide(canonical).c_str(), sizeof(szFile));

   // Initialize ofn options
   ZeroMemory(&ofn, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFile = szFile;
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFilter = L"YAML\0*.yaml\0JSON\0*.json\0All\0*.*\0";
   ofn.nFilterIndex = 1;
   ofn.lpstrFileTitle = NULL;
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = NULL; // Inherits from lpstrFile
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

   // Display the Open dialog box.
   if (GetSaveFileNameW(&ofn) == TRUE)
   {
#pragma warning(disable : 6054)
      return WideToUtf8(ofn.lpstrFile);
#pragma warning(default : 6054)
   }

   return "";
}

}; // namespace CubeWorld
