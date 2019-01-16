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

#include <Engine/Core/Maybe.h>
#include <Shared/Helpers/JsonHelper.h>

#include <libfswatch/c++/monitor.hpp>
#include <thread>

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

   //
   // Writes serialized JSON to the given file.
   //
   void SaveJsonToFilename(const nlohmann::json& data);
   
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
   static void WatchFile(JsonFileSync *self);

   //
   // Triggered when the file is changed or moved.
   //
   static void FileWasChanged(const std::vector<fsw::event>& events, void *context);
   
   
   //
   // Main thread body that saves data to the file.
   //
   static void WriteLatestDataToFile(JsonFileSync *self);
   
   //
   // Handles file state changes
   //
   void HandleFSWEvent(fsw::event event);

   std::thread* mFileWatchingThread;
    
   std::thread* mFileSavingThread;
   mutable std::mutex mWritingMutex;   // Mutex used to serialize writing to the file

   // Filename we're tracking with this JsonFileSync object
   std::string mFilename;
   
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
};

}; // Shared

}; // CubeWorld
