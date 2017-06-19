#include <game.hpp>
#include <server_ui.hpp>
#include <csignal>

#define ME "UI"

Game *game; // Should be initialised by init.

void signal_interpreter(int s)
{
  if(s == SIGCONT)
  {
    game -> resume_running();
    return;
  }
  if(s == SIGQUIT)
  {
    game -> stop_running();
    return;
  }
  if((s == SIGINT) || (s == SIGTERM))
  {
    game -> slow_termination();
    return;
  }
  if(s == SIGUSR1)
  {
    game -> demand_stat();
    return;
  }
  if(s == SIGALRM)
  {
    game -> ping_round();
    return;
  }
  
  fprintf(stderr, "Unrecognised signal caught %d.\n", s);
}

void init_server_ui(Logger *log)
{
  log -> record(ME, "Initialising server UI");
  signal(SIGCONT, signal_interpreter); // will resume game.
  signal(SIGQUIT, signal_interpreter); // will pause game.
  signal(SIGINT, signal_interpreter); // will tell game to terminate slowly.
  signal(SIGTERM, signal_interpreter); // will tell game to terminate slowly.
  signal(SIGALRM, signal_interpreter); // will ping clients.
  signal(SIGUSR1, signal_interpreter); // will gather statistics.
}
