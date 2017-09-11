#include <all.hpp>
#include <cstdio>
#include <server.hpp>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <cctype>
#include <thread>
#include <math.hpp>
#include <strings.hpp>
#include <csignal>
#include <chrono>
#include <ctime>
#include <cmath>
#include <poll.h>

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

#define ME "Captain Jack"

void cb(Websocket_Con *ws, string x)
{
  printf("callback called by {%d} with [%s]\n", ws -> id, x.c_str());
  if(x.length() == 0)
  {
    delete ws;
  }
}

int host(Logger *log)
{
  struct sockaddr_in serv_addr;
  int socketid = socket(AF_INET, SOCK_STREAM, 0);
  int aux = 1;
  setsockopt(socketid, SOL_SOCKET, SO_REUSEADDR, &aux, sizeof(int));
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(1633);
  if(bind(socketid, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
    log -> record(ME, "Failed to bind socket.");
    fprintf(stderr, "Failed to bind socket. Exiting\n");
    exit(0);
  }
  listen(socketid, SV_MAX_LISTEN);
  signal(SIGPIPE, SIG_IGN); // will ignore SIGPIPE generated by socket activity
  log -> record(ME, "Server starting.");
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);
  int news;
  while(true){
    news = accept(socketid, (struct sockaddr *) &cli_addr, &clilen);
    if (news < 0)
    {
      log -> record(ME, "Server closing.");
      break;
    }
    log -> record(ME, "fresh meat.");
  char *buffer = new char[SV_ACTUAL_BUF_SIZE]();
  safe_read_http_all(news, buffer);
  Websocket_Con *ws = new Websocket_Con(news, buffer, log, cb);
  new thread(&Websocket_Con::handle, ws);
  long wait = 999999999;
  while(wait--);
  ws -> writews("Ya bet they do!");
  while(1);
  }
  return 123;
}

int hijack(Logger *log)
{
  new thread(host, log);
  long wait = 999999999;
  while(wait--);
  printf("hi jack!\n");
  char *buffer = new char[SV_ACTUAL_BUF_SIZE]();
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    log -> record(ME, "Failed to initialise socket");
  }
  struct hostent *server;
  struct sockaddr_in server_address;
  server = gethostbyname("localhost");
  if(server == NULL)
  {
    log -> record(ME, "can't find server");
  }
  bzero((char *) &server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
       (char *)&server_address.sin_addr.s_addr,
       server->h_length);
  server_address.sin_port = htons(1633);
  if(connect(
     sock,
     (struct sockaddr *)&server_address,
     sizeof(server_address)
     ) < 0)
  {
    log -> record(ME, "Failed to connect");
  }
  Websocket_Con *ws = new Websocket_Con(sock, buffer, log, cb);
  ws -> writews("WebSocket rocks");
  string call = "GET / HTTP/1.1\r\nHost: echo.websocket.org\r\n";
  new thread(&Websocket_Con::call, ws, call, "echo");
  wait = 999999999;
  while(wait--);
  while(1);
  return 0;
}