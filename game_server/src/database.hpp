#ifndef DATABASE_H
#define DATABASE_H

#include <block.hpp>
#include <log.hpp>

// TODO figure out how to access the database

void *init_db(const char *db, Logger *log);

Block **load_from_db(long NW, long SE);

void update_db(Block* block);

#endif
