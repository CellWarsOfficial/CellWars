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
    exit(0);
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
      response = response + "Connection: Upgrade, keep-alive\n";
      point = string_seek(comm_buf, "Upgrade:");
      if(point)
      {
        response = response
                 + "Upgrade: "
                 + string_get_next_token(point, STR_WHITE)
                 + '\n';
      }
      point = string_seek(comm_buf, "Sec-WebSocket-Version:");
      if(point)
      {
        response = response
                 + "Sec-WebSocket-Version: "
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
  const char *point;
  char *virtual_buf;
  int len, delta;
  uint32_t mask;
  uint8_t size_desc;
  /*
  log -> record(this_con, "Sending warm welcome message");
  {
    len = form_answer("Welcome to hell, suka blyat.", comm_buf);
    write(s, comm_buf, len);
    memset(comm_buf, 0, len * sizeof(char));
  }
  */
  log -> record(this_con, "Websocket started");
  while((len = read(s, comm_buf, SV_MAX_BUF - SV_PROCESS_ERROR)))
  {
    delta = 1; // skip type
    size_desc = ((uint8_t *)comm_buf)[delta];
    delta += 1; // default 1 byte length
    if(size_desc == 254)
    {
      delta += 2; // 2 more bytes for size
    }
    if(size_desc == 255)
    {
      delta += 8; // 8 more bytes for size
    }
    mask = *((uint32_t *)(comm_buf + delta));
    delta += sizeof(uint32_t); // skip mask
    virtual_buf = comm_buf + delta;
    xormask((uint32_t *)virtual_buf, mask);
    memset(comm_buf + len, 0, SV_PROCESS_ERROR);
    virtual_buf = comm_buf + delta;
    point = string_seek(virtual_buf, "QUERY");
    if(point)
    {
      log -> record(this_con, "Query received");
      int px1, py1, px2, py2;
      point = string_seek(virtual_buf, "px1=");
      px1 = stoi(string_get_next_token(point, STR_WHITE));
      point = string_seek(virtual_buf, "py1=");
      py1 = stoi(string_get_next_token(point, STR_WHITE));
      point = string_seek(virtual_buf, "px2=");
      px2 = stoi(string_get_next_token(point, STR_WHITE));
      point = string_seek(virtual_buf, "py2=");
      py2 = stoi(string_get_next_token(point, STR_WHITE));
      log -> record(this_con, "will query " 
                              + to_string(px1) + " " 
                              + to_string(py1) + " " 
                              + to_string(px2) + " " 
                              + to_string(py2)
                              );
      string to_send = game -> user_want(px1, py1, px2, py2);
      memset(comm_buf, 0, len * sizeof(char));
      len = form_answer(to_send, comm_buf);
      write(s, comm_buf, len);
    }
    point = string_seek(virtual_buf, "UPDATE");
    if(point)
    {
      log -> record(this_con, "Update received");
      int px, py;
      CELL_TYPE t;
      point = string_seek(virtual_buf, "px=");
      px = stoi(string_get_next_token(point, STR_WHITE));
      point = string_seek(virtual_buf, "py=");
      py = stoi(string_get_next_token(point, STR_WHITE));
      point = string_seek(virtual_buf, "t=");
      t = (CELL_TYPE)stoi(string_get_next_token(point, STR_WHITE));
      log -> record(this_con, "will update " 
                              + to_string(px) + " " 
                              + to_string(py) + " " 
                              + to_string(t)
                              );
      game -> user_does(px, py, t);
      memset(comm_buf, 0, len * sizeof(char));
      len = form_answer("1\n", comm_buf);
      write(s, comm_buf, len);
    }
    memset(comm_buf, 0, len * sizeof(char));
  }
  log -> record(this_con, "Websocket terminating");
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

int form_answer(string to_send, char *comm_buf)
{
  int len, delta = 0;
  len = to_send.length();
  comm_buf[delta] = 129; // text frame
  delta += 1; // skip type
  if(len < 126)
  {
    comm_buf[delta] = (char)len; // size fits
    delta += 1; // skip size
  }
  if((len >= 126) && (len <= 65535))
  {
    comm_buf[delta] = 126; // 2 extra bytes
    comm_buf[delta + 1] = (char)((len >> 8) % 256);
    comm_buf[delta + 2] = (char)(len % 256);
    delta += 3; // skip size
  }
  if(len > 65535)
  {
    comm_buf[delta] = 127; // 8 extra bytes, God help us!
    int auxl = len;
    for(int i = 8; i; i--)
    {
      comm_buf[delta + i] = (char)(auxl % 256);
      auxl = auxl >> 8;
    }
    delta += 9; // skip size
  }
  // Heck, mask can be added here!
  to_send.copy(comm_buf + delta, len, 0);
  len += delta;
  return len;
}
