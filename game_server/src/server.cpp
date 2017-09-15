#include <server.hpp>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <cctype>
#include <thread>
#include <math.hpp>
#include <strings.hpp>
#include <csignal>
#include <cmath>
#include <poll.h>

#define ME "server"

Server::Server(int port, DB_conn *db, Player_Manager *pm, Logger *l)
{
  db_info = db;
  player_manager = pm;
  log = l;
  socketid = create_server_socket(port);
  if(socketid < 0)
  {
    fprintf(stderr, "Failed to open server socket on port %d\n", port);
  }
  signal(SIGPIPE, SIG_IGN); // will ignore SIGPIPE generated by socket activity
  log -> record(ME, "Successfully listening to port: " + to_string(port));
}

Server::~Server()
{
  close(socketid);
  log -> record(ME, "Server closed");
}

void Server::start()
{
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
    new thread(&Server::act, this, news);
  }
}

void Server::act(int socket)
{
  char *buffer = new char[SV_ACTUAL_BUF_SIZE]();
  const char *key;
  Websocket_Con *ws;
  safe_read_http_all(socket, buffer);
  key = string_seek(buffer, "/database HTTP/");
  if(key)
  {
    ws = new Websocket_Con(socket, buffer, log, db_info -> get_callback());
    db_info -> subscribe(ws); // TODO: actually implement this stuff - CDN
  }
  else
  {
    ws = new Websocket_Con(socket, buffer, log, player_manager -> get_callback());
    player_manager -> subscribe(ws);
  }
  ws -> handle();
}

void Server::demand_stat()
{
  log -> record(ME, "not implemented");
}

int create_server_socket(int port)
{
  struct sockaddr_in serv_addr;
  int aux = 1;
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    return -1;
  }
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &aux, sizeof(int)))
  {
    return -1;
  }
  memset((char *)&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if(bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))
  {
    return -1;
  }
  listen(sock, SV_MAX_LISTEN);
  return sock;
}

int create_connection_socket(const char *remote, int port)
{
  struct hostent *server;
  struct sockaddr_in serv_addr;
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    return -1;
  }
  server = gethostbyname(remote);
  if(server == NULL)
  {
    return -1;
  }
  memset((char *)&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server -> h_addr, server -> h_length);
  serv_addr.sin_port = htons(port);
  if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
  {
    return -1;
  }
  return sock;
}

int create_connection_socket(string remote, int port)
{
  return create_connection_socket(remote.c_str(), port);
}

int deny_access(int s)
{
  string response = "";
  response = response + SV_HTTP_REDIR + SV_HTTP_DEST + SV_HTTP_SERVER_NAME;
  response = response + SV_HTTP_CLOSE_CON + SV_HTTP_CRLF;
  return safe_write(s, response.c_str(), response.length());
}

int safe_write(int s, const char *buf, int len, int timeout)
{
  int trials = SV_MAX_ATTEMPTS, aux = 0;
  while(trials--)
  {
    if(check_writable(s, timeout))
    {
      aux += write(s, buf + aux, len - aux);
      if(aux == len)
      {
        return 0;
      }
    }
    else 
    {
      break;
    }
  }
  return -1; // broken socket :(
}

int safe_read(int s, char *buf, int len, int timeout)
{
  int trials = SV_MAX_ATTEMPTS, aux = 0;
  while(trials--)
  {
    if(check_readable(s, timeout))
    {
      aux += read(s, buf + aux, len - aux);
      if(aux == len)
      {
        return 0;
      }
    }
    else 
    {
      break;
    }
  }
  return -1; // broken socket :(
}

int check_readable(int s, int timeout)
{
  struct pollfd pfds;
  int poll_return;
  pfds.fd = s;
  pfds.events = POLLIN;
  pfds.revents = 0;
  poll_return = poll(&pfds, 1, timeout);
  return (poll_return > 0) && (pfds.revents & POLLIN);
}

int check_writable(int s, int timeout)
{
  struct pollfd pfds;
  int poll_return;
  pfds.fd = s;
  pfds.events = POLLOUT;
  pfds.revents = 0;
  poll_return = poll(&pfds, 1, timeout);
  return (poll_return > 0) && (pfds.revents & POLLOUT);
}

int safe_read_http_all(int s, char *buf, int timeout)
{
  int trials = SV_MAX_ATTEMPTS;
  int old = 0;
  int fresh;
  int next = 4;
  while(trials--)
  {
    if(safe_read(s, buf + old, next, timeout))
    {
      fresh = next;
      next = 4;
      if(strstr(buf + old + fresh - 1, "\r"))
      {
        next = 3; // might be \n\r\n
      }
      if(strstr(buf + old + fresh - 2, "\r" "\n"))
      {
        next = 2; // might be \r\n
      }
      if(strstr(buf + old + fresh - 3, "\r" "\n" "\r"))
      {
        next = 1; // might be \n
      }
      if(strstr(buf + old + fresh - 4, "\r" "\n" "\r" "\n"))
      {
        return 0; // done
      }
      old += fresh;
      trials += fresh ? 1 : 0;
    }
    else 
    {
      break;
    }
  }
  return -1; // some sort of protocol violation
}

int safe_write(int s, const char *buf, int len)
{
  return safe_write(s, buf, len, SV_WRITE_WAIT_TIMEOUT);
}

int safe_read(int s, char *buf, int len)
{
  return safe_read(s, buf, len, SV_READ_WAIT_TIMEOUT);
}

int check_readable(int s)
{
  return check_readable(s, SV_READ_WAIT_TIMEOUT);
}

int check_writable(int s)
{
  return check_writable(s, SV_WRITE_WAIT_TIMEOUT);
}

int safe_read_http_all(int s, char *buf)
{
  return safe_read_http_all(s, buf, SV_READ_WAIT_TIMEOUT);
}