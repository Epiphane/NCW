//
// JsonFileSync.h
//
// This helper lets you create a .json file that corresponds to a JSON object
//    in memory. Whenever the file changes, it will update the object appropriately,
//    and whenever the object changes, it will send the updates to the file.
//
// Writing to the file is done on a background thread so it doesn't pause the UI,
//    but reading file updates back to client memory is done on the main thread to avoid
//    conflicts.
//
// This helper is super careful to not lose data. If there is a pending update to
//    a file AND we have pending changes we want to save to a file, we will first
//    write a brand new file like "Filename-backup-12.22.2018-10.30.12.json" then
//    update our in-memory JSON object.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <mutex>
#include <thread>
#if !CUBEWORLD_PLATFORM_WINDOWS
#include <libfswatch/c++/monitor.hpp>
#endif

#include <RGBBinding/JsonHelper.h>
#include <RGBFileSystem/Paths.h>

namespace CubeWorld
{

namespace Shared
{

class JsonFileSync
{
public:

   //
   // Constructor that kicks off a fswatch monitor for the given filename, OR
   //    creates a file with the filename if none exists.
   //
   JsonFileSync(const std::string& filename);
   virtual ~JsonFileSync();

   // 
   // Get the JSON from the file. Returns a Failure if the JSON is invalid
   //    or the file is missing.
   //
   Maybe<nlohmann::json> GetJsonFromFile();

   bool DoesFileHaveNewUpdate();

private:
   //
   // Main thread body that watches the file.
   //
   void WatchFile();
   
#if !CUBEWORLD_PLATFORM_WINDOWS
   //
   // Handles file state changes
   //
   void HandleFSWEvent(fsw::event event);

   //
   // Triggered when the file is changed or moved.
   //
   void FileWasChanged(const std::vector<fsw::event>& events, void *context);
#endif
   
   //
   // Main thread body that saves data to the file.
   //
   void WriteLatestDataToFile();

   std::thread mFileWatchingThread;
   std::thread mFileSavingThread;

   std::mutex mSavingMutex;   // Mutex used to serialize writing to the file
   std::condition_variable mCondition;

   // Current idea: when the client changes the state of whatever it's editing,
   //                it blocks as it copies the json structure over to us. Should be doable in 1 frame.
   //                orrrr have the json data be owned by this object, which uses a mutex to make sure nobody's
   //                reading/writing at the same time as each other.
   nlohmann::json mData;

   // Filename we're tracking with this JsonFileSync object
   std::string mFilename;
   
#if !CUBEWORLD_PLATFORM_WINDOWS
   // Filewatching minotaur
   fsw::monitor* mMonitor;
#endif
   
   //
   // Enum describing all the states the file can be in.
   //    Note that when I say "client data" I'm referring to the in-memory
   //    JSON representation of our data.
   //
   typedef enum {
      Idle,               // There is no pending change to the file or the client data.
      FileChanged,        // The file has changed on the filesystem, and we haven't parsed it into client data
      WritingData,        // We are currently writing to the file on a background thread.
      FileMissing,        // There is no longer a file at the path "mFilename". Prompt the user to re-save the file.
   } FileState;
   
   FileState mFileState;

   bool mExiting;
};

}; // Shared

}; // CubeWorld
