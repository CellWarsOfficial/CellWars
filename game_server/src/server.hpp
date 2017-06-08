#ifndef SERVER_H
#define SERVER_H

#include <game.hpp>
#include <log.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

class Server
{
  public:
  Server(int port, Logger *l);
  void start(Game *game);
  private:
  void act(int s);
  int socketid;
  Logger *log;
};

#endif
