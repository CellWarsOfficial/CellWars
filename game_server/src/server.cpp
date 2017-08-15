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

#define ME "server"

WS_info::WS_info(string this_con, int id, int socketid)
{
  this -> this_con = this_con;
  this -> id = id;
  this -> s = socketid;
  this -> agent = 0;
  this -> task = 101;
  this -> ms = 0;
}

void WS_info::drop(Server *server)
{
  close(s);
  server -> erase(id);
  expectation.clear();
  delete this;
}

Server::Server(int port, DB_conn *db, Logger *l)
{
  db_info = db;
  log = l;
  live_conns = 0;
  next_ws = 0;
  conns = 0;
  struct sockaddr_in serv_addr;
  socketid = socket(AF_INET, SOCK_STREAM, 0);
  if(socketid < 0)
  {
    log -> record(ME, "Failed to initialise socket.");
    return;
  }
  int aux = 1;
  setsockopt(socketid, SOL_SOCKET, SO_REUSEADDR, &aux, sizeof(int));
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if(bind(socketid, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
    log -> record(ME, "Failed to bind socket.");
    fprintf(stderr, "Failed to bind socket. Exiting\n");
    exit(0);
  }
  listen(socketid, SV_MAX_LISTEN);
  signal(SIGPIPE, SIG_IGN); // will ignore SIGPIPE generated by socket activity
  log -> record(ME, "Successfully listening to port: " + to_string(port));
}

Server::~Server()
{
  shutdown(socketid, 0);
  check_clients(8); // close all open websockets
  std::this_thread::sleep_for(std::chrono::milliseconds(10000));
  shutdown(socketid, 2);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  close(socketid);
  log -> record(ME, "Server closed");
}

void Server::erase(int id)
{
  server_lock.lock();
  live_conns--;
  monitor.erase(id);
  server_lock.unlock();
}

bool Server::taken(CELL_TYPE t)
{
  server_lock.lock();
  std::map<int, WS_info *>::iterator it;
  for(it = monitor.begin(); it != monitor.end(); it++)
  {
    if(it -> second -> agent == t)
    {
      server_lock.unlock();
      return true;
    }
  }
  server_lock.unlock();
  return false;
}

void Server::forget(int id)
{
  server_lock.lock();
  monitor.erase(id);
  server_lock.unlock();
}

void Server::bcast_message(string message)
{
  server_lock.lock();
  std::map<int, WS_info *>::iterator it;
  for(it = monitor.begin(); it != monitor.end(); it++)
  {
    if(it -> second -> agent)
    {
      new thread(&Server::broadcaster, this, it->second, 1, message);
    }
  }
  server_lock.unlock();
}

void Server::inform(int task, int value)
{
  std::map<int, WS_info *>::iterator it;
  WS_info *w;
  server_lock.lock();
  switch(task)
  {
  case INFORM_UPDATE_MOVES:
    for(it = monitor.begin(); it != monitor.end(); it++)
    {
      if(it -> second -> agent)
      {
        w = it -> second;
        w -> ms = value + ((int) trunc(sqrt(w -> ms))) + get_score(w ->agent);
      }
    }
    break;
  case INFORM_USER_DIES:
    for(it = monitor.begin(); (it != monitor.end()) && (it -> second -> agent != (CELL_TYPE)value); it++);
    if(it != monitor.end())
    {
      it -> second -> agent = 0;
      broadcaster(it -> second, 1, "LOST");
    }
    break;
  default:
    break;
  }
  server_lock.unlock();
}

int Server::get_score(CELL_TYPE t)
{
  string query = "SELECT null FROM agents.grid WHERE user_id=" + to_string(t);
  string result = db_info -> run_query(EXPECT_COUNT, query);
  return stoi(string_get_next_token(result.c_str(), STR_WHITE));
}

void Server::check_clients(uint8_t opcode)
{
  server_lock.lock();
  std::map<int, WS_info *>::iterator it;
  for(it = monitor.begin(); it != monitor.end(); it++)
  {
    new thread(&Server::broadcaster, this, it->second, opcode, "Standard msg");
  }
  server_lock.unlock();
}

void Server::broadcaster(WS_info *w, uint8_t opcode, string to_send)
{
  char *comm_buf = new char[SV_MAX_BUF + 2 * BUF_PROCESS_ERROR];
  int tid;
  w -> write_lock.lock();
  tid = w -> task;
  w -> task += 1;
  if(w -> task == 201)
  {
    w -> task = 101;
  }
  w -> expectation.insert(tid);
  w -> write_lock.unlock();
  log -> record(w -> this_con, "Server sending msg: " + to_string(tid));
  if(
    handle_ws_write(w, comm_buf, opcode, to_string(tid) + ": " + to_send) 
    || (opcode == 8)
    )
  {
    close(w -> s);
    forget(w -> id);
  }
  delete[] comm_buf;
}

void Server::start(Game *game)
{
  log -> record(ME, "Server starting.");
  this -> game = game;
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);
  int news;
  int con_id = 0;
  while(true){
    news = accept(socketid, (struct sockaddr *) &cli_addr, &clilen);
    if (news < 0)
    {
      log -> record(ME, "Server closing.");
      break;
    }
    con_id++;
    conns++;
    new thread(&Server::act, this, news, con_id);
  }
}

void Server::act(int s, int id)
{
  int len, aux;
  bool disable_body = false;
  string this_con = (string)"conn " + to_string(id), response = "";
  char *comm_buf = new char[SV_MAX_BUF + 2 * BUF_PROCESS_ERROR];
  const char *point, *key;
  memset(comm_buf, 0, SV_MAX_BUF * sizeof(char));

  len = read(s, comm_buf, SV_MAX_BUF - 1);
  if(len < 0)
  {
    fprintf(stderr, "Read failed1. handling\n");
    delete comm_buf;
    close(s);
    return;
  }
  log -> record(this_con, "new connection");
  point = string_seek(comm_buf, "GET");
  if(!point)
  {
    point = string_seek(comm_buf, "HEAD");
    disable_body = true;
  }

  if(point) // Expect HTTP get or head
  {
    key = string_seek(comm_buf, "Sec-WebSocket-Key:");
    if(key) // It's a websocket
    {
      log -> record(this_con, "Executing websocket handshakira");
      string token = string_get_next_token(key, STR_WHITE);
      log -> record(this_con, "requesting WS with token: " + token);
      string answer = encode(token);
      log -> record(this_con, "responding with:" + answer);

      response = response + SV_HTTP_SWITCH;
      point = string_seek(comm_buf, "Upgrade:");
      if(point)
      {
        response = response
                 + "Upgrade: "
                 + string_get_next_token(point, STR_WHITE)
                 + SV_HTTP_CRLF;
      }
      response = response + SV_HTTP_UP_CON;
      point = string_seek(comm_buf, "Sec-WebSocket-Version:");
      if(point)
      {
        response = response
                 + "Sec-WebSocket-Version: "
                 + string_get_next_token(point, STR_WHITE)
                 + SV_HTTP_CRLF;
      }
      response = response + "Sec-WebSocket-Accept: " + answer + SV_HTTP_CRLF;
      point = string_seek(comm_buf, "Sec-WebSocket-Protocol:");
      if(point)
      {
        response = response
                 + "Sec-WebSocket-Protocol: "
                 + string_get_next_token(point, STR_WHITE)
                 + SV_HTTP_CRLF;
      }
      response = response + SV_HTTP_CRLF;

      point = response.c_str();
      aux = write(s, point, response.length());
      if(aux < (int)response.length())
      {
        fprintf(stderr, "Write failed1. PANIC\n");
        exit(0);
      }
      memset(comm_buf, 0, len);
      server_lock.lock();
      live_conns--;
      server_lock.unlock();
      hijack_ws(this_con, s, comm_buf);
      delete comm_buf;
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
      response = response + SV_HTTP_OK + SV_HTTP_SERVER_NAME + SV_HTTP_CLOSE_CON;
      response = response + SV_HTTP_CRLF;
    }
    if(expect == 404)
    {
      log -> record(this_con, "404 MISS");
      response = response + SV_HTTP_NOT_FOUND + SV_HTTP_SERVER_NAME + SV_HTTP_CLOSE_CON;
      response = response + SV_HTTP_CRLF;
    }
    aux = write(s, response.c_str(), response.length());
    if(aux < (int)response.length())
    {
      fprintf(stderr, "Write failed2. PANIC\n");
      exit(0);
    }
    if(!disable_body)
    {
      catfile(f, s, comm_buf);
    }
  }
  else
  {
    log -> record(this_con, "405 METHOD NOT ALLOWED");
    response = response + SV_HTTP_ILLEGAL + SV_HTTP_ALLOWED + SV_HTTP_SERVER_NAME;
    response = response + SV_HTTP_CLOSE_CON + SV_HTTP_CRLF;
    aux = write(s, response.c_str(), response.length());
    if(aux < (int)response.length())
    {
      log -> record(this_con, "Write failed3. Ignoring");
    }
  }
  log -> record(this_con, "Ending");
  delete comm_buf;
  close(s);
  return;
}

void Server::hijack_ws(string this_con, int s, char *comm_buf)
{
  int msg_id, my_id;
  string aux;
  log -> record(this_con, "Websocket started");
  server_lock.lock();
  live_conns++; next_ws++; my_id = next_ws;
  WS_info *w = new WS_info(this_con, my_id, s);
  monitor[my_id] = w;
  server_lock.unlock();
  string msg;
  const char *virtual_buf, *point;
  while(1)
  {
    msg = get_ws_msg(w, comm_buf);
    virtual_buf = msg.c_str();
    aux = string_get_next_token(virtual_buf, ":");
    if((aux == "") || (!is_num(aux)))
    {
      log -> record(this_con, "Protocol violation or closure, dropping");
      w -> drop(this);
      return;
    }
    else
    {
      msg_id = stoi(aux);
      if(msg_id == 0)
      {
        // act based on pong
        continue;
      }
      if(msg_id > 100)
      {
        w -> write_lock.lock();
        log -> record(this_con, "Client responding to " + to_string(msg_id));
        if((w -> expectation.find(msg_id)) == (w -> expectation.end()))
        {
          log -> record(this_con, "Unexpected message received, dropping");
          w -> drop(this);
          return;
        }
        w -> write_lock.unlock();
        continue;
      }
      point = string_seek(virtual_buf, "QUERY");
      if(point)
      {
        log -> record(this_con, "Query received");
        if(serve_query(w, aux, point, comm_buf))
        {
          w -> drop(this);
          return;
        }
      }
      point = string_seek(virtual_buf, "CAPITAL");
      if(point)
      {
        log -> record(this_con, "Capitalist query received");
        if(serve_capitals(w, aux, point, comm_buf))
        {
          w -> drop(this);
          return;
        }
      }
      point = string_seek(virtual_buf, "UPDATE");
      if(point)
      {
        if(serve_update(w, aux, point, comm_buf))
        {
          w -> drop(this);
          return;
        }
      }
      point = string_seek(virtual_buf, "SCORE");
      if(point)
      {
        if(serve_score(w, aux, point, comm_buf))
        {
          w -> drop(this);
          return;
        }
      }
      point = string_seek(virtual_buf, "DETAILS");
      if(point)
      {
        if(serve_details(w, aux, point, comm_buf))
        {
          w -> drop(this);
          return;
        }
      }
      point = string_seek(virtual_buf, "PICK");
      if(point)
      {
        if(serve_pick(w, aux, point, comm_buf))
        {
          w -> drop(this);
          return;
        }
      }
    }
  }
  log -> record(this_con, "Websocket terminating");
  w -> drop(this);
  return;
}

string Server::get_ws_msg(WS_info *w, char *comm_buf)
{
  string res = "";
  bool fin = false;
  uint8_t opcode;
  int mask, len, delta, tlen, tsize, aux;
  long size_desc;
  char *virtual_buf;
  while(!fin)
  {
    len = 0;
    while(len < 2)
    { // read just type and legnth for now
      aux = read(w -> s, comm_buf + len, 2 - len);
      if(aux < 0)
      {
        log -> record(w -> this_con, "Failed read length");
        return "";
      }
      len = len + aux;
    }
    opcode = comm_buf[0];
    fin = (opcode & 128);
    opcode = opcode & 31; // 00001111
    switch(opcode)
    {
    case 0 :
    case 1 :
      break; // either continuation or text frame, whatever
    case 2 :
      log -> record(w -> this_con, "Declaring binary data, dropping");
      return "";
    case 3 :
    case 4 :
    case 5 :
    case 6 :
    case 7 :
      log -> record(w -> this_con, "Unrecognised action, dropping");
      return "";
//controll frames
    case 8 :
      log -> record(w -> this_con, "Websocket closed by client.");
      return "";
    case 9 :
    case 10 :
      break; // ping-pong, will be handled just b4 returning.
    default :
      log -> record(w -> this_con, "Unrecognised action, dropping");
      return "";
    }
    delta = 1;
    size_desc = (long)(*((uint8_t *)(comm_buf + delta)));
    mask = size_desc & 128;
    size_desc = size_desc % 128;
    delta = 2;
    while(len < size_desc + delta)
    { // read some more because we can
      aux = read(w -> s, comm_buf + len, size_desc + delta - len);
      if(aux < 0)
      {
        log -> record(w -> this_con, "Failed read more");
        return "";
      }
      len = len + aux;
    }
    if(size_desc == 126)
    {
      delta = 4;
      size_desc = (long)(*((uint16_t *)(comm_buf + delta)));
    }
    if(size_desc == 127)
    {
      delta = 10;
      size_desc = (long)(*((uint64_t *)(comm_buf + delta)));
    }
    if(mask)
    {
      mask = *((uint32_t *)(comm_buf + delta));
      delta += sizeof(uint32_t); // skip mask
    }
    virtual_buf = comm_buf + delta;
    tsize = size_desc + delta;
    tlen = len;
    while(tlen < tsize)
    {
      if(SV_MAX_BUF - len < BUF_THRESHOLD)
      {
        xormask((uint32_t *)virtual_buf, mask);
        memset(comm_buf + len, 0, BUF_PROCESS_ERROR);
        res = res + virtual_buf;
        memset(comm_buf, 0, sizeof(char) * len);
        len = 0;
        virtual_buf = comm_buf;
      }
      aux = read(w -> s, comm_buf + len, min(SV_MAX_BUF - len, tsize - tlen));
      if(aux < 0)
      {
        log -> record(w -> this_con, "Failed read all");
        return "";
      }
      len = len + aux;
      tlen = tlen + aux;
    }
    xormask((uint32_t *)virtual_buf, mask);
    memset(comm_buf + len, 0, BUF_PROCESS_ERROR);
    res = res + virtual_buf;
    memset(comm_buf, 0, sizeof(char) * len);
    len = 0;
    if(opcode == 9)
    {
      log -> record(w -> this_con, "Ping received, handling");
      if(handle_ws_write(w, comm_buf, 10, res))
      { // write back failure
        fin = 1;
      }
      else
      { // everything's fine
        fin = 0;
      }
      res = "";
    }
    if(opcode == 10)
    {
      log -> record(w -> this_con, "Pong received");
      res = "0"; // there was a pong for maybe a ping I sent.
    }
  }
  log -> record(w -> this_con, "Returning message for handling");
  return res;
}

int Server::handle_ws_write(WS_info *w, char *comm_buf, uint8_t opcode, string to_send)
{
  int len, delta, sofar = 0, slice = SV_MAX_BUF, attempts, aux;
  len = to_send.length();
  w -> write_lock.lock();
  while(sofar < len)
  {
    delta = 0;
    if(SV_MAX_BUF >= len - sofar + BUF_PROCESS_ERROR)
    {
      opcode = 128 + opcode; // fin
      slice = len - sofar;
    }
    comm_buf[delta] = opcode;
    delta = 1; // skip type
    if(slice < 126)
    {
      comm_buf[delta] = (char)slice; // size fits
      delta += 1; // skip size
    }
    if((slice >= 126) && (slice <= 65535))
    {
      comm_buf[delta] = 126; // 2 extra bytes
      comm_buf[delta + 1] = (char)((slice >> 8) % 256);
      comm_buf[delta + 2] = (char)(slice % 256);
      delta += 3; // skip size
    }
    to_send.copy(comm_buf + delta, slice, sofar);
    for(aux = 0, attempts = 0; attempts < SV_MAX_ATTEMPTS; attempts++)
    {
      aux = write(w -> s, comm_buf, slice + delta);
      if(aux > 0)
      {
        delta = delta - aux;
      }
      else
      { // allow some time for digestion
        std::this_thread::sleep_for(std::chrono::milliseconds(SV_DELAY));
      }
      if(aux < 0)
      {
        attempts = SV_MAX_ATTEMPTS;
      }
      if(slice + delta == 0)
      {
        break;
      }
    }
    if(attempts >= SV_MAX_ATTEMPTS)
    {
      w -> write_lock.unlock();
      log -> record(w -> this_con, "Write failed");
      return 1;
    }
    memset(comm_buf, 0, slice + delta);
    sofar += slice;
    opcode = 0; // at this point, it's continuation
  }
  w -> write_lock.unlock();
  return 0;
}

int Server::serve_query(WS_info *w, string taskid, const char *virtual_buf, char *comm_buf)
{
  const char *point;
  bool gf = true;
  int px1 = 0, py1 = 0, px2 = 0, py2 = 0;
  string tok;
  point = string_seek(virtual_buf, "px1=");
  tok = string_get_next_token(point, STR_WHITE);
  if(is_num(tok))
  {
    px1 = stoi(tok);
  }
  else
  {
    log -> record(w -> this_con, "Bad formatting");
    gf = false;
  }
  point = string_seek(virtual_buf, "py1=");
  tok = string_get_next_token(point, STR_WHITE);
  if(is_num(tok))
  {
    py1 = stoi(tok);
  }
  else
  {
    log -> record(w -> this_con, "Bad formatting");
    gf = false;
  }
  point = string_seek(virtual_buf, "px2=");
  tok = string_get_next_token(point, STR_WHITE);
  if(is_num(tok))
  {
    px2 = stoi(tok);
  }
  else
  {
    log -> record(w -> this_con, "Bad formatting");
    gf = false;
  }
  point = string_seek(virtual_buf, "py2=");
  tok = string_get_next_token(point, STR_WHITE);
  if(is_num(tok))
  {
    py2 = stoi(tok);
  }
  else
  {
    log -> record(w -> this_con, "Bad formatting");
    gf = false;
  }
  string to_send = taskid + ": ";
  if(gf)
  {
    log -> record(w -> this_con, "will query "
                            + to_string(px1) + " "
                            + to_string(py1) + " "
                            + to_string(px2) + " "
                            + to_string(py2)
                            );
    string query = "SELECT * FROM agents.grid WHERE x>=" + to_string(px1)
                     + " AND x<=" + to_string(px2) + " AND y>="
                     + std::to_string(py1) + " AND y<=" + to_string(py2);
    to_send = to_send + db_info -> run_query(EXPECT_CLIENT, query);
  }
  else
  {
    to_send = to_send + "0";
  }
  if(handle_ws_write(w, comm_buf, 1, to_send))
  {
    log -> record(w -> this_con, "Websocket failure, terminating");
    return 1;
  }
  return 0;
}

int Server::serve_update(WS_info *w, string taskid, const char *virtual_buf, char *comm_buf)
{
  if(w -> agent == 0)
  {
    log -> record(w -> this_con, "Protocol violation, not picked");
    return 1;
  }
  const char *point;
  bool gf = true;
  int px = 1000000000, py = 1000000000;
  CELL_TYPE t = 0;
  string tok;
  point = string_seek(virtual_buf, "px=");
  tok = string_get_next_token(point, STR_WHITE);
  if(is_num(tok))
  {
    px = stoi(tok);
  }
  else
  {
    log -> record(w -> this_con, "Bad formatting");
    gf = false;
  }
  point = string_seek(virtual_buf, "py=");
  tok = string_get_next_token(point, STR_WHITE);
  if(is_num(tok))
  {
    py = stoi(tok);
  }
  else
  {
    log -> record(w -> this_con, "Bad formatting");
    gf = false;
  }
  point = string_seek(virtual_buf, "t=");
  tok = string_get_next_token(point, STR_WHITE);
  if(is_num(tok))
  {
    t = (CELL_TYPE)stoi(tok);
  }
  else
  {
    log -> record(w -> this_con, "Bad formatting");
    gf = false;
  }
  string to_send = taskid + ": ";
  if(gf)
  {
    int move_cost = game -> compute_m_cost(px, py, t);
    if(move_cost > w -> ms)
    {
      log -> record(w -> this_con, "expensive move "
                              + to_string(px) + " "
                              + to_string(py) + " "
                              + to_string(t) + ", user only has "
                              + to_string(w -> ms)
                              );
      to_send = to_send + "0";
    }
    else
    {
      log -> record(w -> this_con, "will update "
                              + to_string(px) + " "
                              + to_string(py) + " "
                              + to_string(t) + " "
                              + to_string(w -> agent)
                              );
      if(game -> user_does(px, py, t, w -> agent))
      {
        to_send = to_send + "1";
        w -> ms -= move_cost;
      }
      else
      {
        to_send = to_send + "0";
      }
    }
  }
  else
  {
    to_send = to_send + "0";
  }
  if(handle_ws_write(w, comm_buf, 1, to_send))
  {
    log -> record(w -> this_con, "Websocket failure, terminating");
    return 1;
  }
  return 0;
}

int Server::serve_score(WS_info *w, string taskid, const char *virtual_buf, char *comm_buf)
{
  const char *point;
  bool gf = true;
  CELL_TYPE t;
  string tok;
  point = string_seek(virtual_buf, "t=");
  tok = string_get_next_token(point, STR_WHITE);
  if(is_num(tok))
  {
    t = (CELL_TYPE)stoi(tok);
  }
  else
  {
    log -> record(w -> this_con, "Bad formatting");
    gf = false;
  }
  string to_send = taskid + ": ";
  if(gf)
  {
    log -> record(w -> this_con, "will show score of "
                            + to_string(t)
                            );
    to_send = to_send + to_string(get_score(t));
  }
  else
  {
    to_send = to_send + "0";
  }
  if(handle_ws_write(w, comm_buf, 1, to_send))
  {
    log -> record(w -> this_con, "Websocket failure, terminating");
    return 1;
  }
  return 0;
}

int Server::serve_details(WS_info *w, string taskid, const char *virtual_buf, char *comm_buf)
{
  string to_send = taskid + ": ";
  log -> record(w -> this_con, "Showing details.");
  to_send = to_send + "1 ";
  to_send = to_send + game -> getdets();
  if(handle_ws_write(w, comm_buf, 1, to_send))
  {
    log -> record(w -> this_con, "Websocket failure, terminating");
    return 1;
  }
  return 0;
}

int Server::serve_capitals(WS_info *w, string taskid, const char *virtual_buf, char *comm_buf)
{
  string to_send = taskid + ": ";
  log -> record(w -> this_con, "Showing details.");
  to_send = to_send + game -> getcaps();
  if(handle_ws_write(w, comm_buf, 1, to_send))
  {
    log -> record(w -> this_con, "Websocket failure, terminating");
    return 1;
  }
  return 0;
}

int Server::serve_pick(WS_info *w, string taskid, const char *virtual_buf, char *comm_buf)
{
  if(w -> agent != 0)
  {
    log -> record(w -> this_con, "Protocol violation, repicking");
    return 1;
  }
  const char *point;
  bool gf = true;
  CELL_TYPE t;
  string tok;
  point = string_seek(virtual_buf, "t=");
  tok = string_get_next_token(point, STR_WHITE);
  if(is_num(tok))
  {
    t = (CELL_TYPE)stoi(tok);
  }
  else
  {
    log -> record(w -> this_con, "Bad formatting");
    gf = false;
  }
  string to_send = taskid + ": ";
  if((gf) && (t != 0) && (!taken(t)))
  {
    log -> record(w -> this_con, "player is picking agent "
                            + to_string(t)
                            );
    w -> agent = t;
    to_send = to_send + to_string(w -> id); // my precious
  }
  else
  {
    to_send = to_send + "0";
  }
  if(handle_ws_write(w, comm_buf, 1, to_send))
  {
    log -> record(w -> this_con, "Websocket failure, terminating");
    return 1;
  }
  return 0;
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
  int n, aux;
  while((n = fread(buf, 1, SV_MAX_BUF - 1, f)) > 0)
  {
    aux = write(s, buf, n);
    if(aux < n)
    {
      fclose(f);
      return;
    }
  }
  fclose(f);
}

void Server::demand_stat()
{
  log -> record(ME, "currently serving "
               + to_string(live_conns)
               + " websockets.");
  log -> record(ME, "Have server a total of "
               + to_string(next_ws)
               + " websockets.");
  log -> record(ME, "Total serves "
               + to_string(conns)
               + " and counting! ;)");
  log -> record(ME, "Below is info about my own little database.");
  db_info -> demand_stat();
}

int deny_access(int s)
{
  string response = "";
  response = response + SV_HTTP_REDIR + SV_HTTP_DEST + SV_HTTP_SERVER_NAME;
  response = response + SV_HTTP_CLOSE_CON + SV_HTTP_CRLF;
  return safe_write(s, response.c_str(), response.length());
}

int safe_write(int s, const char *buf, int len)
{
  int trials = SV_MAX_ATTEMPTS, aux = 0;
  while(trials--)
  {
    aux += write(s, buf + aux, len - aux);
    if(aux == len)
    {
      return 0;
    }
  }
  return -1; // broken socket :(
}