#include <game.hpp>
#include <server.hpp>
#include <server_ui.hpp>
#include <csignal>

#define ME "UI"

Game *bound_game;
Server *bound_server;

void signal_interpreter(int s)
{
  if(s == SIGCONT)
  {
    bound_game -> resume_running();
    return;
  }
  if(s == SIGQUIT)
  {
    bound_game -> stop_running();
    return;
  }
  if((s == SIGINT) || (s == SIGTERM))
  {
    bound_game -> slow_termination();
    return;
  }
  if(s == SIGUSR1)
  {
    bound_game -> demand_stat();
    bound_server -> demand_stat();
    return;
  }
  if(s == SIGALRM)
  {
    return;
  }
  
  fprintf(stderr, "Unrecognised signal caught %d.\n", s);
}

void init_server_ui(Logger *log, Game *game, Server *server)
{
  log -> record(ME, "Initialising server UI");
  bound_game = game;
  bound_server = server;
  signal(SIGCONT, signal_interpreter); // will resume game.
  signal(SIGQUIT, signal_interpreter); // will pause game.
  signal(SIGINT, signal_interpreter); // will tell game to terminate slowly.
  signal(SIGTERM, signal_interpreter); // will tell game to terminate slowly.
  signal(SIGALRM, signal_interpreter); // will ping clients.
  signal(SIGUSR1, signal_interpreter); // will gather statistics.
}
