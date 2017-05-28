#ifndef GAME_H
#define GAME_H
#include <map>
#include <constants.hpp>
#include <block.hpp>

#define GFLAG_running flags&JUST_31_MASK
#define GFLAG_stepped_tick flags&JUST_30_MASK
#define GFLAG_continue flags&JUST_29_MASK

/* Object Game is self contained as a process, and is started using start()
 */

class Game
{
  public:
  Game();
  void *start(void *arg);
  int get_status();
  void start_running();
  void stop_running();
  void slow_termination();
  private:
  FLAG_TYPE flags;
  int gen_to_run;
  int plan_time;
  std::map <long, Block*> super_node;
  void plan(int wait_time);
  void crank(int generations);
};

extern Game game;

#endif
