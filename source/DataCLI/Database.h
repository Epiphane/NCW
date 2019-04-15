// By Thomas Steinke

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <RGBFileSystem/Paths.h>

struct sqlite3;
struct sqlite3_stmt;

namespace CubeWorld
{

class Database {
public:
   struct Blob {
      std::string key;
      std::vector<uint8_t> value;
   };

   //
   // Callback to be performed on a blob entry.
   //
   // Return value is a Maybe<void>. Pass Success to continue execution.
   //
   using BlobCallback = std::function<Maybe<void>(const Blob&)>;

private:
   //
   // A collection of prepared sqlite3 statements. Defined internally.
   //
   struct Statements;

public:
   Database(sqlite3* db, std::unique_ptr<Statements>&& statements);
   ~Database();
   static Maybe<std::unique_ptr<Database>> OpenRead(const std::string& path);

   Maybe<void> EnumerateBlobs(const BlobCallback& callback);

private:
   static Maybe<void> PrepareStatements(sqlite3* db, Statements* stmts);

   sqlite3* mDatabase;

   std::unique_ptr<Statements> mStatements;
};

}; // namespace CubeWorld
