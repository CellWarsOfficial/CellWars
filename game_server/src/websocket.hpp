#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <mutex>
#include <function>
#include <constants.hpp>

class Websocket_Con{
  public:
  int id;
  Websocket_Con(int socket, char *buffer, std::function<void(void *,std::string)> callback);
  ~Websocket_Con();
  void handle(); // used to accept a websocket request
//  void call(); // used to connect to a remote websocket server
  void kill(); // used to close the websocket
  void write(string data);
  void ping(string data);
  std::function<void(std::string)> callback;
  private:
  int pinger;
  string con;
  int socket;
  logger log;
  bool kill;
  std:mutex kill_lock;
  std:mutex ws_lock;
  string *write_buffer;
  char *buffer;
  int buffer_read
  int buffer_write;
  void act(); // main loop, will write if there's something to write.
};

#endif