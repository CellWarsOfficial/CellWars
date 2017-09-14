#ifndef GAME_H
#define GAME_H
#include <constants.hpp>
#include <block.hpp>
#include <log.hpp>
#include <server.hpp>
#include <players.hpp>
#include <database.hpp>
#include <action.hpp>
#include <crank.hpp>
#include <server.hpp>
#include <map>
#include <mutex>
#include <cstdlib>
#include <string>
#include <queue>

#define GFLAG_running (flags&JUST_31_MASK)
#define GFLAG_stepped_tick (flags&JUST_30_MASK)
#define GFLAG_continue (flags&JUST_29_MASK)
#define GFLAG_started (flags&JUST_28_MASK)
#define GFLAG_nodb (flags&JUST_27_MASK)
#define GFLAG_nosv (flags&JUST_26_MASK)

/* Object Game is self contained as a process, and is started using start()
 */
class Player_Manager;

class Game
{
  public:
  Game(DB_conn* db, Player_Manager *pm, Logger *log);
  ~Game();
  void *start(FLAG_TYPE f, int gtc, int w, int bm);
  int get_status();
  string user_want(int px1, int py1, int px2, int py2);
  int user_does(int x, int y, CELL_TYPE t, CELL_TYPE user_type);
  void resume_running();
  void stop_running();
  void slow_termination();
  void ping_round();
  void demand_stat();
  int compute_m_cost(int x, int y, CELL_TYPE t);
  string getdets();
  string getcaps();
  Action *action;
  Block *get_curr_block(int x, int y);
  void user_loses(CELL_TYPE user_type);
  int get_gtc();
  int get_wait();
  private:
  Logger *log;
  Player_Manager *player_manager;
  std::mutex execution_lock;
  std::mutex flag_protection;
  std::mutex crank_lock;
  FLAG_TYPE flags;
  DB_conn* db_info;
  int gen_to_run;
  int curr_gen;
  int plan_time;
  int base_moves;
  std::map <uint64_t, Block*> super_node;
  Block *get_block(int x, int y);
  void check_run();
  void plan_stage(int wait_time);
  void crank_stage(int generations);
  void sync_padding();
  void flush_buf();
  void up_db();
  void load_from_db();
  void clean_up();
  std::map<uint64_t, CELL_TYPE> change_buffer;
  std::map<CELL_TYPE, uint64_t> capitals;
  std::queue<uint64_t> buff_order;
  std::map<CELL_TYPE, uint64_t> rips;
  std::mutex capitals_lock;
};

#endif
