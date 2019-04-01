//
// JsonFileSync.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include <Engine/Logger/Logger.h>

#include "JsonFileSync.h"

#if !CUBEWORLD_PLATFORM_WINDOWS
#include <libfswatch/c++/monitor_factory.hpp>
#include <libfswatch/c/libfswatch.h>
#endif

namespace CubeWorld
{

namespace Shared
{

JsonFileSync::JsonFileSync(const std::string& filename)
   : mFilename(filename)
#if !CUBEWORLD_PLATFORM_WINDOWS
   , mMonitor(nullptr)
#endif
   , mFileState(FileChanged)  // Start at "FileChanged" so we load the JSON file into memory on startup
   , mExiting(false)
{
   mFileWatchingThread = std::thread(std::bind(&JsonFileSync::WatchFile, this));
   mFileSavingThread = std::thread(std::bind(&JsonFileSync::WriteLatestDataToFile, this));
}

JsonFileSync::~JsonFileSync()
{
#if !CUBEWORLD_PLATFORM_WINDOWS
   if (mMonitor) {
      mMonitor->stop();
   }
#endif

   mSavingMutex.lock();
   mExiting = true;
   mSavingMutex.unlock();
   mCondition.notify_one();

   mFileWatchingThread.join();
   mFileSavingThread.join();
}

#pragma mark - Watching and reading FROM file
   
//
// Static function. Starts watching the JSON file here.
//
void JsonFileSync::WatchFile()
{
#if CUBEWORLD_PLATFORM_WINDOWS
   LOG_WARNING("File watching not supported on Windows :(");
#else
   std::vector<std::string> filename = {self->mFilename};
   self->mMonitor = fsw::monitor_factory::create_monitor(system_default_monitor_type, filename, &JsonFileSync::FileWasChanged, self);
   
#ifdef __OSX__
   // noDefer means we get events instantly instead of batched together
   self->mMonitor->set_property("darwin.eventStream.noDefer", "true");
#endif
   
   self->mMonitor->start();
#endif
}
 
#if !CUBEWORLD_PLATFORM_WINDOWS
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
               mSavingMutex.lock();
               mFileState = FileChanged;
               mSavingMutex.unlock();
               mCondition.notify_one();
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
#endif

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
void JsonFileSync::WriteLatestDataToFile()
{
   std::unique_lock<std::mutex> lock{mSavingMutex};

   for (;;) {
      mCondition.wait(lock, [&] { return mExiting || DoesFileHaveNewUpdate(); });
      
      if (mExiting)
      {
         break;
      }

      mFileState = Idle;

      // PANIC
      // Save current Json to a backup file, then reload the new Json and inform the user.
   }
}
   


}; // Shared

}; // CubeWorld
