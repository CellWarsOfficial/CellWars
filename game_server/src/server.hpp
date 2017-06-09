#ifndef SERVER_H
#define SERVER_H

#include <game.hpp>
#include <log.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string>

class Server
{
  public:
  Server(int port, Logger *l);
  ~Server();
  void start(Game *game);
  private:
  void act(int s, int id);
  int socketid;
  Game *game;
  Logger *log;
};

int buffer_parse_detector(const char *b, string pattern);

int buffer_parse_detector(const char *b, char *pattern);

string get_next_token(char *b, int i);

#endif
