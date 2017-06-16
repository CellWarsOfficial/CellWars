#ifndef SERVER_H
#define SERVER_H

#include <game.hpp>
#include <database.hpp>
#include <log.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string>

class Game;

class Server
{
  public:
  Server(int port, DB_conn *db, Logger *l);
  ~Server();
  void start(Game *game);
  void demand_stat();
  private:
  void act(int s, int id);
  void hijack_ws(string this_con, int s, char *comm_buf);
  int socketid;
  DB_conn *db_info;
  Game *game;
  Logger *log;
  int live_conns;
  int conns;
};

FILE *get_404();

FILE *get_file(string file);

void catfile(FILE *f, int s, char *buf);

int form_answer(uint8_t opcode, string s, char *buf);

#endif
