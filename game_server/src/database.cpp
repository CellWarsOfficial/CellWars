#include <database.hpp>
#include <block.hpp>
#include <constants.hpp>

// TODO all functions in here.

void *db_info;

void init_db(const char *db)
{
  printf("Connection to \"%s\" successful\n", db);
}

Block* load_from_db(int NWx, int NWy, int SEx, int SEy)
{
  exit(-1);
}

void update_db(Block* block, int NWx, int NWy, int SEx, int SEy)
{
  exit(-1);
}
