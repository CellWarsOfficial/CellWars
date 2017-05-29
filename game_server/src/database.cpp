#include <database.hpp>
#include <block.hpp>
#include <constants.hpp>
#include <cstdio>
#include <cstdlib>

// TODO all functions in here.

void *init_db(const char *db)
{
  printf("Connection to \"%s\" successful\n", db);
  printf("skipping");
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
