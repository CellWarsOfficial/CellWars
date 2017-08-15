#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <mutex>
#include <functional>
#include <constants.hpp>
#include <log.hpp>
#include <unistd.h>

using namespace std;

class Websocket_Con{
  public:
  int id;
  Websocket_Con(int socket, char *buffer, Logger *log, function<void(void *,string)> callback);
  ~Websocket_Con();
  void handle(); // used to accept a websocket request
//  void call(); // used to connect to a remote websocket server
//  void kill(); // used to close the websocket
  void writews(string data);
  void ping(string data);
  std::function<void(void *,std::string)> callback;
  private:
  bool need_ping;
  string con; // used to identify myself
  int socket;
  Logger *log;

//  mutex kill_lock;
  mutex ws_lock;
  string *write_buffer;
  char *buffer;
  int buffer_read;
  int buffer_write;
  void act(); // main loop, will write if there's something to write.
  void self_terminate();
};

#endif