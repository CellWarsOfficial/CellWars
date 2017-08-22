#include <websocket.hpp>
#include <math.hpp>
#include <strings.hpp>

using namespace std;

/* public functions */

Websocket_Con::Websocket_Con(int socket, char *buffer, Logger *log, std::function<void(void *,std::string)> callback)
{
  this -> id = socket;
  this -> socket = socket;
  this -> buffer = buffer; // becomes my responsibility
  this -> callback = callback;
  this -> log = log;
  this -> write_buffer = new string[WS_MAX_BUF];
  this -> need_ping = false;
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
    return;
  }
  else
  {
    deny_access(socket);
  }
  self_terminate();
}

void Websocket_Con::ping(string data)
{
  ws_lock.lock();
  need_ping = true;
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

void Websocket_Con::act()
{
  // dark magic
}

void Websocket_Con::self_terminate()
{
  this -> callback(this, ""); // tell higher entity to close me
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
      this -> self_terminate();
      return -1;
    }
    memset(buffer, 0, slice + delta);
    sofar += slice;
    opcode = 0; // at this point, it's continuation
  }
  return 0;
}