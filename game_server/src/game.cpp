#include <game.hpp>
#include <all.hpp>
#include <cstdio>
#include <thread>
#include <chrono>

#define ME "Game"

/* Note that Game has default destructor, as clean_up takes care of memory.
 */

Game::Game(DB_conn* db, Logger *log)
{
  this -> log = log;
  db_info = db;
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
  flags = (f & NO_LAST_MASK) | JUST_28_MASK; // unset running and set started.
  gen_to_run = gtc;
  plan_time = w;
  Block **blocks = db_info -> load_from_db(
    compress_xy(MINX, MINY), 
    compress_xy(MAXX, MAXY)
    );
  for(int i = 0; blocks[i]; i++)
  {
    super_node[
      compress_xy((blocks[i]) -> originx, (blocks[i]) -> originy)
      ] = blocks[i];
  }
  delete[] blocks;
  log -> record(ME, "Imported data from database for startup/resume.");
  flags = flags | JUST_29_MASK; // set continue flag
  flag_protection.unlock();

/* Start running the game
 */
  resume_running();
  while(GFLAG_continue)
  {
    check_run();
    plan_stage(plan_time);
    check_run();
    crank_stage(gen_to_run);
  }
  clean_up();
  return NULL;
}

void Game::plan_stage(int wait_time)
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
  up_db();
}

void Game::crank_stage(int generations)
{
  log -> record(ME, "Crank - start");
  std::map<uint64_t,Block*>::iterator i;
  for (i = super_node.begin(); i != super_node.end(); i++)
  {
    dump(i->second);
    crank(i -> second);
    dump(i->second);
  }
  up_db();
  log -> record(ME, "Crank - finish");
}

void Game::up_db()
{
  log -> record(ME, "Updating database");
  std::map<uint64_t,Block*>::iterator i;
  for (i = super_node.begin(); i != super_node.end(); i++)
  {
    db_info -> update_db(i -> second);
  }
  log -> record(ME, "Database updated");
}

/* clean_up assumes FR and database are in sync, as it is normally called
 * after a crank has finished.
 */

void Game::clean_up()
{
  std::map<uint64_t,Block*>::iterator i;
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
