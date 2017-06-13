#include <server.hpp>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <cctype>
#include <thread>
#include <math.hpp>
#include <strings.hpp>

#define ME "server"

Server::Server(int port, Logger *l)
{
  log = l;
  struct sockaddr_in serv_addr;
  socketid = socket(AF_INET, SOCK_STREAM, 0);
  if(socketid < 0)
  {
    log -> record(ME, "Failed to initialise socket.");
    return;
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if(bind(socketid, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
    log -> record(ME, "Failed to bind socket.");
    return;
  }
  listen(socketid, SV_MAX_LISTEN);
  log -> record(ME, "Successfully listening to port: " + to_string(port));
}

Server::~Server()
{
  close(socketid);
  log -> record(ME, "Server closed");
}

void Server::start(Game *game)
{
  this -> game = game;
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);
  int news;
  int con_id = 0;
  while(true){
    news = accept(socketid, (struct sockaddr *) &cli_addr, &clilen);
    if (news < 0) 
    {
      log -> record(ME, "Failed to accept client.");
      continue;
    }
    con_id++;
    new thread(&Server::act, this, news, con_id);
  }
}

void Server::act(int s, int id)
{
  int len;
  string this_con = (string)"conn " + to_string(id);
  char *comm_buf = new char[SV_MAX_BUF];
  const char *point, *key;
  memset(comm_buf, 0, SV_MAX_BUF * sizeof(char));

  len = read(s, comm_buf, SV_MAX_BUF - 1);
  log -> record(this_con, "new connection");
  point = string_seek(comm_buf, "GET");

  if(point) // Expect HTTP protocol
  {
    key = string_seek(comm_buf, "Sec-WebSocket-Key:");
    if(key)
    {
      log -> record(this_con, "Executing websocket handshakira");
      string token = string_get_next_token(key, STR_WHITE);
      log -> record(this_con, "requesting WS with token: " + token);
      string answer = encode(token);
      log -> record(this_con, "responding with:" + answer);

      string response = (string)SV_HTTP_SWITCH + '\n';
      response = response + "Connection: Upgrade\n";
      point = string_seek(comm_buf, "Upgrade:");
      if(point)
      {
        response = response
                 + "Upgrade: "
                 + string_get_next_token(point, STR_WHITE)
                 + '\n';
      }
      response = response + "Sec-WebSocket-Accept: " + answer + '\n';
      point = string_seek(comm_buf, "Sec-WebSocket-Protocol:");
      if(point)
      {
        response = response
                 + "Sec-WebSocket-Protocol: "
                 + string_get_next_token(point, STR_WHITE)
                 + '\n';
      }
      response = response + SV_HTTP_END;

      point = response.c_str();
      write(s, point, strlen(point));
      memset(comm_buf, 0, len);
      hijack_ws(this_con, s, comm_buf);
      return;
    }

    log -> record(this_con, (string)"standard connection");
    string file_path = string_get_next_token(point, STR_WHITE);
    log -> record(this_con, (string)"client asking for " + file_path);
    FILE *f = get_file(file_path);
    int expect = 200;
    if(f == NULL)
    {
      f = get_404();
      expect = 404;
    }

    if(expect == 200)
    {
      log -> record(this_con, "200 HIT");
      write(s, SV_HTTP_OK, strlen(SV_HTTP_OK));
      catfile(f, s, comm_buf);
    }
    if(expect == 404)
    {
      log -> record(this_con, "404 MISS");
      write(s, SV_HTTP_NOT_FOUND, strlen(SV_HTTP_NOT_FOUND));
      catfile(f, s, comm_buf);
    }
  }

  write(s, SV_HTTP_END, 2);
  delete comm_buf;
  close(s);
  return;
}

void Server::hijack_ws(string this_con, int s, char *comm_buf)
{
  log -> record(this_con, "Websocket started");
  while(1);
}

FILE *get_404()
{
  // Deliver file 404
  FILE *f;
  f = fopen(SV_HTML_MISSING, "r");
  check_malloc(f);
  return f;
}

FILE *get_file(string file)
{
  // detect illegal access
  if(file.find("..") != string::npos)
  {
    return NULL;
  }
  // remove GET arguments
  size_t qmark = file.find("?");
  if(qmark != string::npos)
  {
    file.erase(qmark, SV_MAX_BUF);
  }
  // add index.html default
  if(file.back() == '/')
  {
    file = file + "index.html"; 
  }
  // use path to file
  file = (string)SV_HTML_PATH + file;
  // attempt to open
  FILE *f = fopen(file.c_str(), "r");
  return f;
}

void catfile(FILE *f, int s, char *buf)
{
  int n;
  while((n = fread(buf, 1, SV_MAX_BUF - 1, f)) > 0)
  {
    write(s, buf, n);
  }
}
