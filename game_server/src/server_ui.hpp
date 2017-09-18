#ifndef SERVER_UI_H
#define SERVER_UI_H

#include <log.hpp>
#include <game.hpp>
#include <server.hpp>

void signal_interpreter(int s);

void init_server_ui(Logger *log, Game *game, Server *server);

#endif
