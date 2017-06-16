#ifndef DATABASE_H
#define DATABASE_H

#include <block.hpp>
#include <log.hpp>
#include <cstdio>
#include <string>
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
  void rewrite_db(const char *f);
  int safe;
  string run_query(int expectation, string s);
  void insert_query_builder(CELL_TYPE t, int x, int y);
  void demand_stat();
  private:
  char *answer_buf;
  std::mutex db_lock;
  Logger *log;
  const char *address;
  int socketid;
  int q_count;
  int size;
  string constructed_query;
};
#endif
