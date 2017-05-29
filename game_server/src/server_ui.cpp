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
  if(s == SIGSTOP)
  {
    game -> stop_running();
    return;
  }
  if(s == SIGINT)
  {
    game -> slow_termination();
    return;
  }
  fprintf(stderr, "Unrecognised signal caught %d.\n", s);
}

void init_server_ui(Logger *log)
{
  log -> record(ME, "Initialising server UI");
  signal(SIGCONT, signal_interpreter); // will resume game.
  signal(SIGSTOP, signal_interpreter); // will pause game.
  signal(SIGINT, signal_interpreter); // will tell game to terminate slowly.
}
