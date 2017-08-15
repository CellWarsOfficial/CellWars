#include <websocket.hpp>
#include <math.hpp>
#include <strings.hpp>

using namespace std;

Websocket_Con::Websocket_Con(int socket, char *buffer, Logger *log, std::function<void(void *,std::string)> callback)
{
  this -> id = socket;
  this -> socket = socket;
  this -> buffer = buffer; // becomes by responsibility
  this -> callback = callback;
  this -> log = log;
  this -> write_buffer = new string[WS_MAX_BUF];
  this -> need_ping = false;
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

    if(safe_write(this -> socket, response.c_str(), response.length()))
    {
      this -> self_terminate();
      return;
    }
    memset(buffer, 0, SV_MAX_BUF + 2 * BUF_PROCESS_ERROR);
    act();
  }
  else
  {
    deny_access(this -> socket);
  }
  this -> self_terminate();
}

void Websocket_Con::act()
{
  // dark magic
}

void Websocket_Con::writews(string data)
{

}

void Websocket_Con::ping(string data)
{
  need_ping = true;
}

void Websocket_Con::self_terminate()
{
  this -> callback(this, ""); // tell higher entity to close me
}

