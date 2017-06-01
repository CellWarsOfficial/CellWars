#ifndef DATABASE_H
#define DATABASE_H

#include <block.hpp>
#include <log.hpp>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

// TODO figure out how to access the database

class DB_conn
{
  public:
  DB_conn(const char *a, Logger *l);
  Block **load_from_db(long NW, long SE);
  void update_db(Block* block);
  int safe;
  private:
  void* run_query(int expectation, string s);
  Logger *log;
  const char *address;
  int socketid;
  struct hostent *server;
  struct sockaddr_in server_address;
};

struct answer
{
  int row;
  int col;
  CELL_TYPE t;
  struct answer *next;
};
#endif
