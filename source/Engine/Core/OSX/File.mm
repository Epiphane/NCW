// By Thomas Steinke

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include <string>
#include <vector>

#include "../File.h"

namespace CubeWorld
{

std::string OpenFileDialog(
   const std::string& defaultFilename,
   const std::vector<std::string>& fileTypes
)
{
   // Create a File Open Dialog class.
   NSOpenPanel* openDlg = [NSOpenPanel openPanel];
   [openDlg setLevel:CGShieldingWindowLevel()];

   // Set array of file types
   if (fileTypes.size() > 0)
   {
      NSMutableArray* fileTypesArray = [NSMutableArray array];
      for (int i = 0; i < fileTypes.size(); i++)
      {
         [fileTypesArray addObject:[NSString stringWithUTF8String:fileTypes[i].c_str()]];
      }

      [openDlg setAllowedFileTypes:fileTypesArray];
   }

   // Enable options in the dialog.
   [openDlg setCanChooseFiles:YES];
   [openDlg setAllowsMultipleSelection:FALSE];
   [openDlg setDirectoryURL:[NSURL URLWithString:[NSString stringWithUTF8String:defaultFilename.c_str()]]];

   // Display the dialog box. If the OK pressed, process the files.
   if ([openDlg runModal] == NSModalResponseOK)
   {
      NSArray* files = [openDlg URLs];
      for(int i = 0; i < [files count]; i++)
      {
         return std::string([[[files objectAtIndex:i] path] UTF8String]);
      }
   }

   return "";
}

std::string SaveFileDialog(
   const std::string& defaultFilename
)
{
   NSSavePanel* saveDlg = [NSSavePanel savePanel];
   [saveDlg setLevel:CGShieldingWindowLevel()];
   [saveDlg setDirectoryURL:[NSURL URLWithString:[NSString stringWithUTF8String:defaultFilename.c_str()]]];

   if ([saveDlg runModal] == NSModalResponseOK)
   {
      NSURL *saveURL = [saveDlg URL];
      NSString* path = [saveURL path];
      return std::string([path UTF8String]);
   }

   return "";   
}

}; // namespace CubeWorld