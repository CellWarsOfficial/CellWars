#include <websocket.hpp>
#include <math.hpp>
#include <strings.hpp>
#include <poll.h>

using namespace std;

/* public functions */

Websocket_Con::Websocket_Con(int socket, char *buffer, Logger *log, std::function<void(void *,std::string)> callback)
{
  this -> id = socket;
  this -> socket = socket;
  this -> buffer = buffer; // becomes my responsibility
  this -> callback = callback;
  this -> log = log;
  write_buffer = new string[WS_MAX_BUF];
  need_ping = false;
  sent_ping = false;
  buffer_size = WS_MAX_BUF;
  wrapped = false;
  con = "" + socket;
  buffer_read = 0;
  buffer_write = 0;
}

Websocket_Con::~Websocket_Con()
{
  delete[] buffer;
  delete[] write_buffer;
}

void Websocket_Con::handle()
{
  const char *key;
  string response = "";
  key = string_seek(buffer, "Sec-WebSocket-Key:");
  if(key) // It's a websocket
  {
    log -> record(con, "Executing websocket handshakira");
    string token = string_get_next_token(key, STR_WHITE);
    log -> record(con, "requesting WS with token: " + token);
    string answer = encode(token);
    log -> record(con, "responding with:" + answer);

    response = response + SV_HTTP_SWITCH;
    key = string_seek(buffer, "Upgrade:");
    if(key)
    {
      response = response
               + "Upgrade: "
               + string_get_next_token(key, STR_WHITE)
               + SV_HTTP_CRLF;
    }
    response = response + SV_HTTP_UP_CON;
    key = string_seek(buffer, "Sec-WebSocket-Version:");
    if(key)
    {
      response = response
               + "Sec-WebSocket-Version: "
               + string_get_next_token(key, STR_WHITE)
               + SV_HTTP_CRLF;
    }
    response = response + "Sec-WebSocket-Accept: " + answer + SV_HTTP_CRLF;
    key = string_seek(buffer, "Sec-WebSocket-Protocol:");
    if(key)
    {
      response = response
               + "Sec-WebSocket-Protocol: "
               + string_get_next_token(key, STR_WHITE)
               + SV_HTTP_CRLF;
    }
    response = response + SV_HTTP_CRLF;

    if(safe_write(socket, response.c_str(), response.length()))
    {
      self_terminate();
      return;
    }
    memset(buffer, 0, SV_MAX_BUF + 2 * BUF_PROCESS_ERROR);
    act();
    self_terminate();
    return;
  }
  else
  {
    deny_access(socket);
  }
  self_terminate();
}

void Websocket_Con::ping()
{
  ws_lock.lock();
  need_ping = !sent_ping; // don't schedule to send another ping if one is sent already
  ws_lock.unlock();
}

void Websocket_Con::writews(string data)
{
  ws_lock.lock();
  write_buffer[buffer_read] = data;
  buffer_read++;
  if(buffer_read == buffer_size)
  {
    buffer_read = 0;
    wrapped = true;
  }
  ws_lock.unlock();
}

/* private functions */
/* act priority in functioning
 * 1 if I have something to write, I will write it.
 * 2 if I need to send a ping, I will if there's nothing to write.
 * 3 if I need to read, I will if there's something to read within 10 seconds.
 * 4 if there's nothing to read, but no ping on the way, I'll send a ping
 * 5 if there's nothing to read within 10 seconds again, i'll terminate myself
 * analyse - 
 * if I get a ping message, I will respond straight away.
 * if I get a pong message, I will mark sent_ping as false.
 * if I can't tell at a first glance what to do with a message, I'll send it
 *   to the boss so he can have a look at it.
 */

void Websocket_Con::act()
{
  string read_data;
  while(true)
  {
    ws_lock.lock();
    if(buffer_read != buffer_write)
    { // 1
      ws_lock.unlock();
      log -> record(con, "Writing data");
      if(emit(WS_OPCODE_TXT, write_buffer[buffer_write]))
      {
        return;
      }
      ws_lock.lock();
      buffer_write++;
      if(buffer_write == buffer_size)
      {
        buffer_write = 0;
        wrapped = false;
      }
      ws_lock.unlock();
      continue;
    }
    if(need_ping)
    { // 2
      need_ping = false;
      sent_ping = true;
      ws_lock.unlock();
      log -> record(con, "Sending ping");
      if(emit(WS_OPCODE_PING, "Cells are pretty cool man!"))
      {
        return;
      }
      continue;
    }
    ws_lock.unlock();
    log -> record(con, "See if there's something to read");
    if(poll(NULL, 1, WS_INPUT_WAIT_TIMEOUT)) // TODO: obvious
    { // 3
      log -> record(con, "There is");
      if(parse(&read_data))
      {
        return;
      }
      if(analyse(read_data))
      {
        return;
      }
      continue;
    }
    if(!sent_ping)
    { // 4
      ping();
      continue;
    }
    return; // 5 - user timeout, didn't respond to my ping
  }
}

int Websocket_Con::parse(string *result_container)
{
  return 0;
}

int Websocket_Con::analyse(string previous_data)
{
  return 0;
}

int Websocket_Con::emit(uint8_t opcode, string to_send)
{
  int len, delta, sofar = 0, slice = SV_MAX_BUF;
  len = to_send.length();
  while(sofar < len)
  {
    delta = 0;
    if(SV_MAX_BUF >= len - sofar + BUF_PROCESS_ERROR)
    {
      opcode = 128 + opcode; // fin
      slice = len - sofar;
    }
    buffer[delta] = opcode;
    delta = 1; // skip type
    if(slice < 126)
    {
      buffer[delta] = (char)slice; // size fits
      delta += 1; // skip size
    }
    if((slice >= 126) && (slice <= 65535))
    {
      buffer[delta] = 126; // 2 extra bytes
      buffer[delta + 1] = (char)((slice >> 8) % 256);
      buffer[delta + 2] = (char)(slice % 256);
      delta += 3; // skip size
    }
    to_send.copy(buffer + delta, slice, sofar);

    if(safe_write(socket, buffer, slice + delta))
    {
      return -1;
    }
    memset(buffer, 0, slice + delta);
    sofar += slice;
    opcode = 0; // at this point, it's continuation
  }
  return 0;
}

void Websocket_Con::self_terminate()
{
  this -> callback(this, ""); // tell higher entity to close me
}
