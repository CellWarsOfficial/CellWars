#ifndef SERVER_UI_H
#define SERVER_UI_H

#include <log.hpp>

extern Game *game;

void signal_interpreter(int s);

void init_server_ui(Logger *log);

#endif
