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
#include <mutex>
#include <constants.hpp>

class Game;

class WS_info
{
  public:
  WS_info(string this_con, int id, int socketid);
  void drop();
  std::mutex write_lock;
  string this_con;
  int id;
  int s;
  CELL_TYPE agent;
};

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
  string get_ws_msg(WS_info *w, char *comm_buf);
  int handle_ws_write(WS_info *w, char *comm_buf, uint8_t opcode, string to_send);
  int serve_query(WS_info *w, string taskid, const char *point, char *comm_buf);
  int serve_update(WS_info *w, string taskid, const char *point, char *comm_buf);
  int serve_score(WS_info *w, string taskid, const char *point, char *comm_buf);
  int serve_pick(WS_info *w, string taskid, const char *point, char *comm_buf);
  int socketid;
  DB_conn *db_info;
  Game *game;
  Logger *log;
  int live_conns;
  int conns;
  std::mutex server_lock;
};

FILE *get_404();

FILE *get_file(string file);

void catfile(FILE *f, int s, char *buf);

#endif
