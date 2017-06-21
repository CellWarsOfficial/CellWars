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
#include <map>
#include <set>
#include <constants.hpp>

class Game; class Server;

class WS_info
{
  public:
  WS_info(string this_con, int id, int socketid);
  void drop(Server *server);
  std::mutex write_lock;
  int task;
  string this_con;
  int id;
  int s;
  CELL_TYPE agent;
  int ms;
  std::set<int> expectation;
};

class Server
{
  public:
  Server(int port, DB_conn *db, Logger *l);
  ~Server();
  void start(Game *game);
  void bcast_message(string message);
  void check_clients(uint8_t opcode);
  void erase(int id);
  void demand_stat();
  void inform(int task, int value);
  int get_score(CELL_TYPE t);
  private:
  void act(int s, int id);
  void hijack_ws(string this_con, int s, char *comm_buf);
  string get_ws_msg(WS_info *w, char *comm_buf);
  int handle_ws_write(WS_info *w, char *comm_buf, uint8_t opcode, string to_send);
  void broadcaster(WS_info *w, uint8_t opcode, string to_send);
  void forget(int id);
  int serve_query(WS_info *w, string taskid, const char *point, char *comm_buf);
  int serve_update(WS_info *w, string taskid, const char *point, char *comm_buf);
  int serve_score(WS_info *w, string taskid, const char *point, char *comm_buf);
  int serve_pick(WS_info *w, string taskid, const char *point, char *comm_buf);
  int serve_details(WS_info *w, string taskid, const char *point, char *comm_buf);
  int serve_capitals(WS_info *w, string taskid, const char *point, char *comm_buf);
  int socketid;
  DB_conn *db_info;
  Game *game;
  Logger *log;
  int live_conns;
  int next_ws;
  int conns;
  std::mutex server_lock;
  std::map <int, WS_info *> monitor;
};

FILE *get_404();

FILE *get_file(string file);

void catfile(FILE *f, int s, char *buf);

#endif
