#include <server.hpp>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>

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
}

void Server::start(Game *game)
{
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);
  int news;
  while(true){
    news = accept(socketid, (struct sockaddr *) &cli_addr, &clilen);
    if (news < 0) 
    {
      log -> record(ME, "Failed to accept client.");
      continue;
    }
    act(news);
  }
}

void Server::act(int s)
{
  int n, i;
  string file_path;
  char *comm_buf = new char[SV_MAX_BUF];
  read(s, comm_buf, SV_MAX_BUF - 1);
  log -> record(ME, "new connection ");
  for(i = 0; (comm_buf[i] != '/') && (comm_buf[i]); i++);
  if(!comm_buf[i])
  {
    log -> record(ME, "dropped");
    close(s);
    return;
  }
  file_path = SV_HTML_PATH;
  if(comm_buf[i + 1] == ' ')
  {
    log -> record(ME, "client asking for index");
    file_path = file_path + "/index.html";
  }
  else
  {
    log -> record(ME, "client asking for file");
    for(; (comm_buf[i] != ' '&& (comm_buf[i])); i++)
    {
      file_path = file_path + comm_buf[i];
    }
    if(!comm_buf[i])
    {
      log -> record(ME, "dropped");
      close(s);
      return;
    }
  }
  log -> record(ME, "responding with " + file_path);
  FILE *f = fopen(file_path.c_str(), "r");
  if(f)
  {
    write(s, SV_HTTP_OK, 17);
    while((n = fread(comm_buf, 1, SV_MAX_BUF - 1, f)) > 0)
    {
      write(s, comm_buf, n);
    }
    write(s, SV_HTTP_END, 2);
    fclose(f);
  }
  else
  {
    write(s, SV_HTTP_NOT_FOUND, 24);
    write(s, SV_HTTP_END, 2);
  }
  delete comm_buf;
  close(s);
}
