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
#include <constants.hpp>

using namespace std;

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
  int socketid;
  DB_conn *db_info;
  Game *game;
  Logger *log;
};

int create_server_socket(int port);

int create_connection_socket(const char *remote, int port);

int create_connection_socket(string remote, int port);

int deny_access(int s);

int safe_write(int s, const char *buf, int len, int timeout);

int safe_read(int s, char *buf, int len, int timeout);

int check_readable(int s, int timeout);

int check_writable(int s, int timeout);

int safe_read_http_all(int s, char *buf, int timeout);

int safe_write(int s, const char *buf, int len);

int safe_read(int s, char *buf, int len);

int check_readable(int s);

int check_writable(int s);

int safe_read_http_all(int s, char *buf);

#endif