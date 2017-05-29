#ifndef GAME_H
#define GAME_H
#include <constants.hpp>
#include <block.hpp>
#include <log.hpp>
#include <map>
#include <mutex>
#include <cstdlib>

#define GFLAG_running flags&JUST_31_MASK
#define GFLAG_stepped_tick flags&JUST_30_MASK
#define GFLAG_continue flags&JUST_29_MASK
#define GFLAG_started flags&JUST_28_MASK

/* Object Game is self contained as a process, and is started using start()
 */

class Game
{
  public:
  Game(void *ptr, Logger *log);
  void *start(uint32_t f, int gtc, int w);
  int get_status();
  void resume_running();
  void stop_running();
  void slow_termination();
  private:
  Logger *log;
  std::mutex execution_lock;
  std::mutex flag_protection;
  FLAG_TYPE flags;
  void* db_info;
  int gen_to_run;
  int plan_time;
  std::map <long, Block*> super_node;
  void check_run();
  void plan(int wait_time);
  void crank(int generations);
  void clean_up();
};

#endif
