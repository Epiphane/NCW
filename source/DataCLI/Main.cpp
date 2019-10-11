//
// DataCLI - CLI for interacting with Not CubeWorld data
//

#include <iostream>
#include <stdlib.h>
#include <string>

#include <sqlite3.h>

#include <RGBLogger/Logger.h>
#include <RGBLogger/DebugLogger.h>
#include <RGBLogger/StdoutLogger.h>

#include "Console.h"
#include "ConvertModelCommand.h"
#include "ConvertDocumentCommand.h"
#include "Database.h"
#include "DumpCommand.h"

using namespace CubeWorld;

int main(int argc, char **argv)
{
   Logger::StdoutLogger::Instance();
   Logger::DebugLogger::Instance();

   if (argc < 2)
   {
      Console::Log("Usage: datacli COMMAND\n");
      Console::Log("Commands:      convert-model\n");
      Console::Log("               convert-document\n");
      Console::Log("               dump");
      return 1;
   }

   Maybe<std::string> result;
   std::string command = argv[1];
   if (command == "convert-model")
   {
      ConvertModelCommand cmd{};
      result = cmd.Run(argc - 2, argv + 2);
   }
   else if (command == "convert-document")
   {
      ConvertDocumentCommand cmd{};
      result = cmd.Run(argc - 2, argv + 2);
   }
   else if (command == "dump")
   {
      DumpCommand cmd{};
      result = cmd.Run(argc - 2, argv + 2);
   }
   else
   {
      Console::Log("Command not recognized: {command}", command);
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
