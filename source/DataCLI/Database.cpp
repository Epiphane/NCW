// By Thomas Steinke

#include <sqlite3.h>

#include <Engine/Core/Paths.h>
#include <Engine/Core/Scope.h>

#include "Database.h"

namespace CubeWorld
{

struct Database::Statements {
   //
   // SELECT key, value FROM blobs
   //
   sqlite3_stmt* listBlobs;

   //
   // SELECT key, value FROM blobs WHERE key=?
   //
   sqlite3_stmt* selectBlob;
};

Failure SqliteFailure(int rc, const std::string& action)
{
   return Failure{"Failed to %1: sqlite3 error code %2: %3", action, rc, sqlite3_errstr(rc)};
}

Maybe<std::unique_ptr<Database>> Database::OpenRead(const std::string& path)
{
   sqlite3* db;

   int rc = sqlite3_open(path.c_str(), &db);
   if (rc != SQLITE_OK)
   {
      return SqliteFailure(rc, "open database");
   }

   std::unique_ptr<Statements> statements = std::make_unique<Statements>();

   if (auto result = PrepareStatements(db, statements.get()); !result)
   {
      return result.Failure().WithContext("Failed to prepare statements");
   }

   return std::make_unique<Database>(db, std::move(statements));
}

Database::Database(sqlite3* db, std::unique_ptr<Statements>&& statements) : mDatabase(db), mStatements(std::move(statements))
{
}

Database::~Database()
{
   sqlite3_close(mDatabase);
}

Maybe<void> Database::PrepareStatements(sqlite3* db, Statements* stmts)
{
   const int flags = SQLITE_PREPARE_PERSISTENT;

   const char kSelectAllBlobs[] = "SELECT key, value FROM blobs";
   if (int rc = sqlite3_prepare_v3(db, kSelectAllBlobs, -1, flags, &stmts->listBlobs, nullptr);  rc != SQLITE_OK)
   {
      return SqliteFailure(rc, "prepare select all blobs query");
   }

   const char kSelectBlob[] = "SELECT key, value FROM blobs WHERE key=?";
   if (int rc = sqlite3_prepare_v3(db, kSelectBlob, -1, flags, &stmts->selectBlob, nullptr); rc != SQLITE_OK)
   {
      return SqliteFailure(rc, "prepare select blob query");
   }

   return Success;
}

Maybe<void> Database::EnumerateBlobs(const BlobCallback& callback)
{
   int rc = sqlite3_step(mStatements->listBlobs);
   CUBEWORLD_SCOPE_EXIT([&] { sqlite3_reset(mStatements->listBlobs); })

   while (rc == SQLITE_ROW)
   {
      Blob blob;
      blob.key = std::string((char*)sqlite3_column_text(mStatements->listBlobs, 0));
      int size = sqlite3_column_bytes(mStatements->listBlobs, 1);
      const uint8_t* data = (uint8_t*)sqlite3_column_blob(mStatements->listBlobs, 1);
      blob.value.assign(data, data + size);

      Maybe<void> result = callback(blob);
      if (!result)
      {
         return result.Failure().WithContext("Failed during enumeration");
      }

      rc = sqlite3_step(mStatements->listBlobs);
   }

   if (rc != SQLITE_DONE)
   {
      return SqliteFailure(rc, "step mSelectBlobs");
   }

   return Success;
}

}; // namespace CubeWorld
