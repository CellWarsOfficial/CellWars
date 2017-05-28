#include <game.hpp>
#include <all.hpp>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>

Game::Game()
{
  flags = 0; // Important because it unsets running flag
}

int Game::get_status()
{
  return GFLAG_running;
}

void Game::start_running()
{
  flags = flags | JUST_LAST_MASK;
  //TODO implement some sort of syncronisation here to ACTUALLY pause thread.
}

void Game::stop_running()
{
  flags = flags & NO_LAST_MASK;
  //TODO implement some sort of syncronisation here to ACTUALLY pause thread.
}

void Game::slow_termination()
{
  flags = flags & NO_29_MASK; // unset continue flag
}

void *Game::start(void *args)
{
  if(GFLAG_running)
  {
    fprintf(stderr, "Game already running, start() invoked twice, ignoring.\n");
    return (void*)1;
  }

  Block *blocks;
  uint32_t *callstack = (uint32_t*) args;
  flags = callstack[0] & NO_LAST_MASK;
  gen_to_run = (int) callstack[1];
  plan_time = (int) callstack[2];
  for(blocks = load_from_db(MINX, MINY, MAXX, MAXY); blocks; blocks++)
  {
    super_node[compress_xy(blocks->originx, blocks->originy)] = blocks;
  }

  start_running();
  flags = flags | JUST_29_MASK; // set continue flag
  while(GFLAG_continue)
  {
    plan(plan_time);
    crank(gen_to_run);
  }
  return NULL;
}

void Game::plan(int wait_time)
{
  // TODO instruct network manager to receive changes from users
  if(GFLAG_stepped_tick)
  {
    stop_running(); // wait for outside interaction to resume.
  }
  else
  {
    std::this_thread::sleep_for(std::chrono::seconds(wait_time));
  }
  
  // TODO instruct network manager to ignore changes from users
  // TODO update database
}

void Game::crank(int generations)
{
  // TODO call crank
  // TODO update database
}
