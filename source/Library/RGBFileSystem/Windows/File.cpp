// By Thomas Steinke

#include <algorithm>
#include <string>
#include <vector>

#include <Windows.h>
#include <GL/includes.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

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
   OPENFILENAME ofn;    // common dialog box structure
   char szFile[260];    // buffer for file name
   HWND hwnd = nullptr; // owner window

   std::string canonical = Paths::Canonicalize(defaultFile);
   std::replace(canonical.begin(), canonical.end(), '/', '\\');

   strncpy_s(szFile, canonical.c_str(), sizeof(szFile));

   // Initialize ofn options
   ZeroMemory(&ofn, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFile = szFile;
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFilter = "YAML\0*.yaml\0JSON\0*.json\0All\0*.*\0";
   ofn.nFilterIndex = 1;
   ofn.lpstrFileTitle = NULL;
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = NULL; // Inherits from lpstrFile
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

   // Display the Open dialog box. 
   if (GetOpenFileName(&ofn) == TRUE)
   {
      return std::string(ofn.lpstrFile);
   }

   return "";
}

std::string SaveFileDialog(
   const std::string& defaultFile
)
{
   // Inspiration: https://docs.microsoft.com/en-us/windows/desktop/dlgbox/using-common-dialog-boxes
   OPENFILENAME ofn;    // common dialog box structure
   char szFile[260];    // buffer for file name
   HWND hwnd = nullptr; // owner window

   std::string canonical = Paths::Canonicalize(defaultFile);
   std::replace(canonical.begin(), canonical.end(), '/', '\\');

   strncpy_s(szFile, canonical.c_str(), sizeof(szFile));

   // Initialize ofn options
   ZeroMemory(&ofn, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFile = szFile;
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFilter = "YAML\0*.yaml\0JSON\0*.json\0All\0*.*\0";
   ofn.nFilterIndex = 1;
   ofn.lpstrFileTitle = NULL;
   ofn.nMaxFileTitle = 0;
   ofn.lpstrInitialDir = NULL; // Inherits from lpstrFile
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

   // Display the Open dialog box. 
   if (GetSaveFileName(&ofn) == TRUE)
   {
      return std::string(ofn.lpstrFile);
   }

   return "";
}

}; // namespace CubeWorld

