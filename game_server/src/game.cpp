#include <game.hpp>
#include <all.hpp>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <chrono>

Game::Game()
{
  execution_lock.lock();
  flags = 0; // Important because it unsets running flag
}

int Game::get_status()
{
  flag_protection.lock();
  return GFLAG_running;
  flag_protection.unlock();
}

void Game::resume_running()
{
  if(!get_status())
  {
    flag_protection.lock();
    flags = flags | JUST_LAST_MASK;
    flag_protection.unlock();
    execution_lock.unlock();
  }
}

void Game::stop_running()
{
  if(get_status())
  {
    execution_lock.lock();
    flag_protection.lock();
    flags = flags & NO_LAST_MASK;
    flag_protection.unlock();
  }
}

void Game::slow_termination()
{
  flag_protection.lock();
  flags = flags & NO_29_MASK; // unset continue flag
  flag_protection.unlock();
}

/* By returning from check_run, I am guaranteed the right to continue execution.
 */

void Game::check_run()
{
  execution_lock.lock();
  execution_lock.unlock();
}

void *Game::start(void *args)
{
  if(GFLAG_started)
  {
    fprintf(stderr, "Game already running, start() invoked twice, ignoring.\n");
    return (void*)1;
  }

/* Initialise the game
 */
  flag_protection.lock();
  flags = flags | JUST_28_MASK; // set started flag
  Block *blocks;
  uint32_t *callstack = (uint32_t*) args;
  flags = callstack[0] & NO_LAST_MASK;
  gen_to_run = (int) callstack[1];
  plan_time = (int) callstack[2];
  for(blocks = load_from_db(MINX, MINY, MAXX, MAXY); blocks; blocks++)
  {
    super_node[compress_xy(blocks->originx, blocks->originy)] = blocks;
  }
  flags = flags | JUST_29_MASK; // set continue flag
  flag_protection.unlock();

/* Start running the game
 */
  resume_running();
  while(GFLAG_continue)
  {
    check_run();
    plan(plan_time);
    check_run();
    crank(gen_to_run);
  }
  clean_up();
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

void Game::clean_up()
{
  // TODO free any allocated memory, safely destroy locks, empty maps etc.
}
