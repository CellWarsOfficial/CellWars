#include <game.hpp>
#include <all.hpp>
#include <cstdio>
#include <thread>
#include <chrono>

#define ME "Game"

/* Note that Game has default destructor, as clean_up takes care of memory.
 */

Game::Game(void* ptr, Logger *log)
{
  this -> log = log;
  db_info = ptr;
  execution_lock.lock();
  flags = 0; // Important because it unsets running flag
}

int Game::get_status()
{
  flag_protection.lock();
  int tmp = GFLAG_running;
  flag_protection.unlock();
  return tmp;
}

void Game::resume_running()
{
  if(!get_status())
  {
    flag_protection.lock();
    flags = flags | JUST_LAST_MASK;
    flag_protection.unlock();
    execution_lock.unlock();
    log -> record(ME, "Resume");
    return;
  }
  log -> record(ME, "Resume called but already running");
}

void Game::stop_running()
{
  if(get_status())
  {
    execution_lock.lock();
    flag_protection.lock();
    flags = flags & NO_LAST_MASK;
    flag_protection.unlock();
    log -> record(ME, "Stop");
    return;
  }
  log -> record(ME, "Stop called but already stopped");
}

void Game::slow_termination()
{
  flag_protection.lock();
  flags = flags & NO_29_MASK; // unset continue flag
  flag_protection.unlock();
  log -> record(ME, "Registered termination request");
}

/* By returning from check_run, I am guaranteed the right to continue execution.
 */

void Game::check_run()
{
  execution_lock.lock();
  execution_lock.unlock();
}

void *Game::start(FLAG_TYPE f, int gtc, int w)
{
  if(GFLAG_started)
  {
    fprintf(stderr, "Game already running, start() invoked twice, ignoring.\n");
    return NULL;
  }

/* Initialise the game
 */
  flag_protection.lock();
  log -> record(ME, 
      "Creating new game with gtc " + to_string(gtc) + " and w" + to_string(w)
      );
  Block *blocks;
  flags = (f & NO_LAST_MASK) | JUST_28_MASK; // unset running and set started.
  gen_to_run = gtc;
  plan_time = w;
  for(blocks = load_from_db(MINX, MINY, MAXX, MAXY); blocks; blocks++)
  {
    super_node[compress_xy(blocks->originx, blocks->originy)] = blocks;
  }
  log -> record(ME, "Imported data from database for startup/resume.");
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
    log -> record(ME, "Planning time - debug step - waiting for UI");
    stop_running();
    check_run(); // wait for outside interaction to resume.
  }
  else
  {
    log -> record(ME, "Planning time - waiting " + to_string(wait_time));
    std::this_thread::sleep_for(std::chrono::seconds(wait_time));
  }
  log -> record(ME, "Planning time - timeout ");
  
  // TODO instruct network manager to ignore changes from users
  // TODO update database
}

void Game::crank(int generations)
{
  log -> record(ME, "Crank - start");
  std::this_thread::sleep_for(std::chrono::seconds(1));
  // TODO call crank
  // TODO update database
  log -> record(ME, "Crank - finish");
}

/* clean_up assumes FR and database are in sync, as it is normally called
 * after a crank has finished.
 */

void Game::clean_up()
{
  std::map<long,Block*>::iterator i;
  log -> record(ME, "Terminating");
  for (i = super_node.begin(); i != super_node.end(); i++)
  {
    delete i -> second;
  }
  super_node.clear();
/* Reset everything, allowing the game to be recycled.
 */
  execution_lock.lock();
  flags = 0; // Important because it unsets running flag
}
