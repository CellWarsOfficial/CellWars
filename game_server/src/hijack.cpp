#include <all.hpp>
#include <cstdio>

#define ME "Captain Jack"

void cb(Websocket_Con *ws, string x)
{
  printf("[%s]\n", x.c_str());
}

int hijack(Logger *log)
{
  printf("hi jack!\n");
  char *buffer = new char[SV_MAX_BUF]();
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    log -> record(ME, "Failed to initialise socket");
  }
  struct hostent *server;
  struct sockaddr_in server_address;
  server = gethostbyname("echo.websocket.org");
  if(server == NULL)
  {
    log -> record(ME, "can't find server");
  }
  bzero((char *) &server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
       (char *)&server_address.sin_addr.s_addr,
       server->h_length);
  server_address.sin_port = htons(80);
  if(connect(
     sock,
     (struct sockaddr *)&server_address,
     sizeof(server_address)
     ) < 0)
  {
    log -> record(ME, "Failed to connect");
  }
  Websocket_Con *ws = new Websocket_Con(sock, buffer, log, cb);
  string call = "GET / HTTP/1.1\r\nHost: echo.websocket.org\r\n";
  new thread(&Websocket_Con::call, ws, call, "echo");
  long wait = 999999999;
  while(wait--);
  // ws -> writews("penguins");
  while(1);
  return 0;
}