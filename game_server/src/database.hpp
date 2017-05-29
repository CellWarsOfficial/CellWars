#ifndef DATABASE_H
#define DATABASE_H

#include <block.hpp>

// TODO figure out how to access the database

void *init_db(const char *db);

Block* load_from_db(int NWx, int NWy, int SEx, int SEy);

void update_db(Block* block, int NWx, int NWy, int SEx, int SEy);

#endif
