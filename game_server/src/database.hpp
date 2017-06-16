#ifndef DATABASE_H
#define DATABASE_H

#include <block.hpp>
#include <log.hpp>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <spp.hpp>
#include <mutex>


// TODO figure out how to access the database

class DB_conn
{
  public:
  DB_conn(const char *a, Logger *l);
  ~DB_conn();
/*
  Block **load_from_db(uint64_t NW, uint64_t SE);
  void update_db(Block* block);
*/
  void rewrite_db(const char *f);
  int safe;
  string run_query(int expectation, string s);
  private:
  char *answer_buf;
  std::mutex db_lock;
  Logger *log;
  const char *address;
  int socketid;
};

struct answer
{
  int row;
  int col;
  CELL_TYPE t;
  struct answer *next;
};
#endif
