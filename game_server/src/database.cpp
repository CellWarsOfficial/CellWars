#include <database.hpp>
#include <block.hpp>
#include <constants.hpp>
#include <cstdio>
#include <cstdlib>

// TODO all functions in here.

#define ME "Database"

void *init_db(const char *db, Logger *log)
{
  log -> record(ME, (string)"Connection to database \"" + db + "\" successful");
  void* db_info = (void*)new int; // duck typing
  return db_info;
}

Block* load_from_db(int NWx, int NWy, int SEx, int SEy)
{
  printf("database load not implemented\n");
  printf("skipping");
  return NULL;
}

void update_db(Block* block, int NWx, int NWy, int SEx, int SEy)
{
  printf("database load not implemented\n");
  printf("skipping");
}
