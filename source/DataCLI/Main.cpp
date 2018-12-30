//
// DataCLI - CLI for interacting with Not CubeWorld data
//

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>

#include <sqlite3.h>

#include <Engine/Logger/Logger.h>
#include <Engine/Logger/DebugLogger.h>
#include <Engine/Logger/StdoutLogger.h>

#include "Console.h"
#include "ConvertCommand.h"
#include "Database.h"
#include "DumpCommand.h"

using namespace CubeWorld;

int main(int argc, char **argv)
{
   Logger::StdoutLogger::Instance();
   Logger::DebugLogger::Instance();

   if (argc < 2)
   {
      Console::Log("Usage: datacli COMMAND");
      return 1;
   }

   Maybe<std::string> result;
   std::string command = argv[1];
   if (command == "convert")
   {
      ConvertCommand cmd{};
      result = cmd.Run(argc - 2, argv + 2);
   }
   else if (command == "dump")
   {
      DumpCommand cmd{};
      result = cmd.Run(argc - 2, argv + 2);
   }
   else
   {
      Console::Log("Command not recognized: %1", command);
      return 1;
   }

   if (!result)
   {
      Console::Log(result.Failure().GetMessage());
      return 1;
   }

   Console::Log(*result);
   return 0;
}
