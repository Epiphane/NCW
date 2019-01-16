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
   mWritingMutex.lock(); // Lock the mWritingMutex on the MAIN THREAD. When we want
                         //    the file-writing thread to run, unlock it from the main thread.

   mFileWatchingThread = new std::thread(JsonFileSync::WatchFile, this);
   mFileSavingThread = new std::thread(JsonFileSync::WriteLatestDataToFile, this);
}

#pragma mark - Watching and reading FROM file
   
//
// Static function. Starts watching the JSON file here.
//
void JsonFileSync::WatchFile(JsonFileSync* self)
{
   std::vector<std::string> filename = {self->mFilename};
   fsw::monitor* new_monitor = fsw::monitor_factory::create_monitor(system_default_monitor_type, filename, &JsonFileSync::FileWasChanged, self);
   
#ifdef __OSX__
   // noDefer means we get events instantly instead of batched together
   new_monitor->set_property("darwin.eventStream.noDefer", "true");
#endif
   
   new_monitor->start();
}
 
//
// Looks at the flags on a given event and handles them based on the current state of the syncer.
//
void JsonFileSync::HandleFSWEvent(fsw::event event) 
{
   // Unfortunately, events don't always match exactly what happened. OS X seems to 
   //    be fond of "rename" events instead of "move" or "delete" events,
   //    so all we can do is note that the file changed somehow and check
   //    that it still exists.
   //
   for (fsw_event_flag flag : event.get_flags()) {
      switch (flag) {
         case MovedFrom:
         case Removed:
         case Renamed:
         case IsDir:
         case MovedTo:
         case Created:
         case Updated:
            if (mFileState == FileMissing) {
               break;
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
// Static function. Called by libfswatch whenever the file is moved or changed.
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
   return mFileState == FileChanged;
}
   
//
// Should be called from the main thread. Parses and returns the latest JSON data,
//    or a Failure state if the data is missing or corrupt.
//
Maybe<nlohmann::json> JsonFileSync::GetJsonFromFile()
{
   return Shared::GetJsonFromFile(mFilename);
}
   
//
// Static function. Blocks until new data comes in to write to the file.
//
void JsonFileSync::WriteLatestDataToFile(JsonFileSync* self)
{
   // Should immediately stop.
   self->mWritingMutex.lock();
   
   
}
   
void JsonFileSync::SaveJsonToFilename(const nlohmann::json& data)
{
   if (mFileState == FileChanged) {
      // PANIC
      // Save current Json to a backup file, then reload the new Json and inform the user.
      
   }
}


}; // Shared

}; // CubeWorld
