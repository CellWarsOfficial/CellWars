#ifndef DATABASE_H
#define DATABASE_H

#include <block.hpp>
#include <log.hpp>

// TODO figure out how to access the database

class DB_conn
{
  public:
  DB_conn(const char *a, Logger *l);
  Block **load_from_db(long NW, long SE);
  void update_db(Block* block);
  private:
  void* run_query(string s);
  Logger *log;
  const char *address;
};
#endif
