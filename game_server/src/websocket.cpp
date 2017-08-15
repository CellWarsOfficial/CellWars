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
  int aux;
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

    key = response.c_str();
    aux = write(this -> socket, key, response.length());
    if(aux < (int)response.length())
    { // TODO: export
      fprintf(stderr, "Write failed1. PANIC\n");
      exit(0);
    }
    memset(buffer, 0, SV_MAX_BUF + 2 * BUF_PROCESS_ERROR);
    act();
  }
  else // obsolete, 301
  { // TODO: export
    response = response + SV_HTTP_REDIR + SV_HTTP_DEST + SV_HTTP_SERVER_NAME;
    response = response + SV_HTTP_CLOSE_CON + SV_HTTP_CRLF;
    key = response.c_str();
    aux = write(this -> socket, key, response.length());
    if(aux < (int)response.length())
    { // TODO: export
      fprintf(stderr, "Write failed1. PANIC\n");
      exit(0);
    }
  }
  this -> callback(this, ""); // tell higher entity to close delete me
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

}