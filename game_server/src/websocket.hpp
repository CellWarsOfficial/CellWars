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
  Websocket_Con(int socket, char *buffer, Logger *log, function<void(Websocket_Con *,string)> callback);
  ~Websocket_Con();
  void handle(); // used to accept a websocket request
  void call(string http_call, string protocol); // used to connect to a remote websocket server
  void kill(); // used to close the websocket
  void writews(string data);
  void ping();
  std::function<void(Websocket_Con *,std::string)> callback;
  void act(); // main loop, will write if there's something to write.
  private:
  bool need_ping;
  bool sent_ping;
  string ping_msg;
  string con; // used to identify myself
  int socket;
  Logger *log;
  mutex ws_lock;
  mutex kill_lock;
  string *write_buffer;
  char *buffer;
  int buffer_size;
  int buffer_read;
  int buffer_write;
  string last_msg;
  uint8_t last_opcode;
  bool last_fin;
  bool wrapped;
  bool applymask;
  int parse();
  int analyse();
  int emit(uint8_t opcode, string to_send);
  void self_terminate();
};

#endif