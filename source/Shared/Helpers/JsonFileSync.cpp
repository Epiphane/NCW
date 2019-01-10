//
// JsonFileSync.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "JsonFileSync.h"

#include <libfswatch/c++/monitor_factory.hpp>
#include <libfswatch/c/libfswatch.h>

namespace CubeWorld
{

namespace Shared
{

JsonFileSync::JsonFileSync(const std::string& filename)
   : mFileWatchingThread(nullptr)
   , mFilename(filename)
   , mFileState(FileChanged)  // Start at "FileChanged" so we load the JSON file into memory on startup
{
   mFileWatchingThread = new std::thread(JsonFileSync::WatchFile, this);
}

//
// Static function. Starts watching the JSON file here.
//
void JsonFileSync::WatchFile(JsonFileSync* self)
{
   std::vector<std::string> filename = {self->mFilename};
   fsw::monitor* new_monitor = fsw::monitor_factory::create_monitor(system_default_monitor_type, filename, &JsonFileSync::FileWasChanged, self);
   new_monitor->start();
}
 
//
// Looks at the flags on a given event and handles them based on the current state of the syncer.
//
void JsonFileSync::HandleFSWEvent(fsw::event event) 
{
   for (fsw_event_flag flag : event.get_flags()) {
      switch (flag) {
         case MovedFrom:
         case Removed:
         case Renamed:
         case IsDir:
            // File has been removed, or somehow changed into a directory (??)
            mFileState = FileMissing;
            break;
         case MovedTo:
         case Created:
         case Updated:
            if (mFileState == ClientChanged || FileClientConflict) {
               mFileState = FileClientConflict;
            }
            else {
               mFileState = FileChanged;
            }
            break;
         case NoOp:
         case PlatformSpecific:
         case OwnerModified:
         case AttributeModified:
         case IsFile:
         case IsSymLink:
         case Link:
            // We don't care about these events >:(
            break;
         case Overflow:
            assert(false && "Somehow event queue overflowed. PANIC!");
            break;
      }
   }
}

//
// Static function. Called whenever the file is moved or changed.
//    Note that "context" is the "self" passed in from WatchFile.
//
void JsonFileSync::FileWasChanged(const std::vector<fsw::event> &events, void* context)
{
   JsonFileSync* self = (JsonFileSync*)context;

   printf("yeet %s\n", self->mFilename.c_str());

   for (fsw::event event : events) {
      self->HandleFSWEvent(event);
   }
}

bool JsonFileSync::DoesFileHaveNewUpdate()
{
   if (mFileState == FileChanged || mFileState == FileClientConflict) {
      return true;
   }
   else {
      return false;
   }
}


}; // Shared

}; // CubeWorld
