#include <server.hpp>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <cctype>
#include <thread>

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

int buffer_parse_detector(const char *b, const char *pattern)
{
  int i = 0, j = 0;
  while(b[i])
  {
    if(b[i] == pattern[j])
    {
      j++;
      if(pattern[j] == 0)
      {
        return i + 1;
      }
    }
    else
    {
      j = 0;
    }
    i++;
  }
  return -1;
}

int buffer_parse_detector(const char *b, string pattern)
{
  return buffer_parse_detector(b, pattern.c_str());
}

void Server::act(int s, int id)
{
  int n, i, m, key, up_p, new_prot_p;
  string file_path, token, up_m, new_prot_m;
  string this_con = ME + to_string(id);
  char *comm_buf = new char[SV_MAX_BUF];
  FILE *f;
  bzero(comm_buf, SV_MAX_BUF);
  m = read(s, comm_buf, SV_MAX_BUF - 1);
  log -> record(this_con, "new connection");
  i = buffer_parse_detector(comm_buf, "GET");
  if(i >= 0) // no get = ignore straight away
  {
// what's the client looking for
    file_path = get_next_token(comm_buf, i);
    if(file_path.compare("/") == 0)
    {
      log -> record(this_con, "client asking for index");
      file_path = (string)SV_HTML_PATH + "/index.html";
    }
    else
    {
      log -> record(this_con, "client asking for file " + file_path);
      if(buffer_parse_detector(file_path.c_str(), "..") != -1)
      {
        log -> record(this_con, "client hacking, managing");
        file_path = "/not_found.html";
      }
      file_path = (string)SV_HTML_PATH + file_path;
    }
// does he want to upgrade to ws?
    key = buffer_parse_detector(comm_buf, "Sec-WebSocket-Key:");
    if(key >= 0)
    {
      token = get_next_token(comm_buf, key);
      up_p = buffer_parse_detector(comm_buf, "Upgrade:");
      if(up_p >= 0)
      {
        up_m = get_next_token(comm_buf, up_p);
      }
      new_prot_p = buffer_parse_detector(comm_buf, "Sec-WebSocket-Protocol:");
      if(new_prot_p >= 0)
      {
        new_prot_m = get_next_token(comm_buf, new_prot_p);
      }
      goto up_ws;
    }

// finished parsing
    bzero(comm_buf, m);
// deliverr the file
    f = fopen(file_path.c_str(), "r");
    if(f)
    {
      log -> record(this_con, "file found");
      write(s, SV_HTTP_OK, strlen(SV_HTTP_OK));
write_now:
      while((n = fread(comm_buf, 1, SV_MAX_BUF - 1, f)) > 0)
      {
        write(s, comm_buf, n);
      }
      fclose(f);
    }
    else
    {
      log -> record(this_con, "file not found");
      write(s, SV_HTTP_NOT_FOUND, strlen(SV_HTTP_NOT_FOUND));
      file_path = (string)SV_HTML_PATH + "/not_found.html";
      f = fopen(file_path.c_str(), "r");
      goto write_now;
    }
  }
// update to ws if required
up_ws:
  if(key >= 0)
  {
    // Solve handshake
    bzero(comm_buf, SV_MAX_BUF);
    log -> record(this_con, "Executing websocket handshakira");
    log -> record(this_con, "token is: " + token);
    string response = (string)SV_HTTP_SWITCH + '\n';
    string magic = "????";
    response = response + "Connection: Upgrade\n";
    if(up_p >= 0)
    {
      response = response + "Upgrade: " + up_m + '\n';
    }
    response = response + "Sec-WebSocket-Accept: " + magic + '\n';
    if(new_prot_p >= 0)
    {
      response = response + "Sec-WebSocket-Protocol: " + new_prot_m + '\n';
    }
    response = response + SV_HTTP_END;
    // Send handshake
    write(s, response.c_str(), strlen(response.c_str()));
  }
// be done with it
  write(s, SV_HTTP_END, 2);
  delete comm_buf;
  close(s);
}

string get_next_token(char *b, int i)
{
  string result = "";
  for(; (isspace(b[i]) && (b[i])); i++);
  for(; (!isspace(b[i]) && (b[i])); i++)
  {
    result = result + b[i];
  }
  return result;
}
