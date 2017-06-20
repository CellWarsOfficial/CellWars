#include <game.hpp>
#include <all.hpp>
#include <cstdio>
#include <thread>
#include <chrono>

#define ME "Game"

/* Note that Game has default destructor, as clean_up takes care of memory.
 */

Game::Game(DB_conn* db, Server *server, Logger *log)
{
  this -> log = log;
  this -> server = server;
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

void *Game::start(FLAG_TYPE f, int gtc, int w, int bm)
{
  if(GFLAG_started)
  {
    fprintf(stderr, "Game already running, start() invoked twice, ignoring.\n");
    return NULL;
  }

/* Initialise the game
 */
  flag_protection.lock();
  curr_gen = 0;
  log -> record(ME,
      "Creating new game with gtc " + to_string(gtc) + " and w" + to_string(w)
      );
  flags = (f & NO_LAST_MASK) | JUST_28_MASK; // unset running and set started.
  gen_to_run = gtc;
  plan_time = w;
  base_moves = bm;
  action = new Crank();
  if(!GFLAG_nodb)
  {
    load_from_db();
    log -> record(ME, "Imported data from database for startup/resume.");
  }
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
  if(server)
  {
    server -> bcast_message("CRANKFIN");
  }
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
  if(!GFLAG_nodb)
  {
    up_db();
  }
  if(server)
  {
    server -> bcast_message("TIMEOUT");
    server -> inform(INFORM_UPDATE_MOVES, base_moves);
  }
}

void Game::crank_stage(int generations)
{
  crank_lock.lock();
  flush_buf();
  if(!GFLAG_nodb)
  {
    up_db();
  }
  log -> record(ME, "Crank - start");
  std::map<uint64_t,Block*>::iterator i;
  for (i = super_node.begin(); i != super_node.end(); i++)
  {
    action -> crank_for(i -> second, gen_to_run);
  }
  curr_gen += gen_to_run;
  sync_padding();
  if(!GFLAG_nodb)
  {
    up_db();
  }
  log -> record(ME, "Crank - finish");
  crank_lock.unlock();
}

Block *Game::get_curr_block(int x, int y)
{
  Block *curr_block;
  int o_x = find_block_origin(x);
  int o_y = find_block_origin(y);
  std::map<uint64_t, Block*>::iterator find_res;
  find_res = super_node.find(compress_xy(o_x, o_y));
  if(find_res != super_node.end())
  {
    curr_block = find_res->second;
  }
  else
  {
    curr_block = new Block(o_x, o_y);
    super_node[compress_xy(o_x, o_y)] = curr_block;
  }
  return curr_block;
}

int Game::compute_m_cost(int x, int y, CELL_TYPE t)
{
  return 5;
}

void Game::flush_buf()
{
  log -> record(ME, "Sending the change buffer");
  int x, y;
  CELL_TYPE t;
  Block *curr_block;
  std::map<uint64_t, CELL_TYPE>::iterator buff_it;
  for(buff_it = change_buffer.begin(); buff_it != change_buffer.end(); buff_it++)
  {
    x = get_x(buff_it->first);
    y = get_y(buff_it->first);
    t = buff_it->second;
    curr_block = get_curr_block(x, y);
    curr_block->set(curr_block->rectify_x(x), curr_block->rectify_y(y), t);
  }
  change_buffer.clear();
  sync_padding();
  log -> record(ME, "Sent the change buffer");
}

void Game::sync_padding()
{
  std::map<uint64_t,Block*>::iterator i, find_res;
  Block *b, *ans;
  void *new_blocks = NULL;
  int dx, dy, region;
  for (i = super_node.begin(); i != super_node.end(); i++)
  {
    b = i -> second;
    for(dx = -1, region = 0; dx <= 1; dx++)
    {
      for(dy = -1; dy <= 1; dy++)
      {
        if((dx == 0) && (dy == 0))
        {
          continue;
        }
/* This iteration only works because of the way regions are defined inside
 * constants.hpp file. Otherwise, too many ifs.
 */
        if(b -> border_changes[region])
        {
          find_res = super_node.find(b -> get_xy_relative(dx, dy));
          if(find_res != super_node.end())
          {
            ans = find_res -> second;
            ans -> sync_with(b, region); // tell, don't ask
          }
          else
          {
/* Can't add the block straight away without breaking/trashing iterator.
 */
            ans = new Block(b -> get_x_relative(dx)
                           ,b -> get_y_relative(dy)
                           );
            ans -> sync_with(b, region);
            ans -> duck = new_blocks;
            new_blocks = (void*) ans;
          }
        }
        b -> border_changes[region] = 0; // reset region changes.
        region++;
      }
    }
  }
  for(; new_blocks; new_blocks = b -> duck, b -> duck = 0)
  {
    b = (Block*) new_blocks;
    super_node[compress_xy(b -> originx, b -> originy)] = b;
  }
}

void Game::up_db()
{
  std::map<uint64_t,Block*>::iterator blk_i;
  int i, j;
  Block *block;
  log -> record(ME, "Updating database");
  db_info -> run_query(NO_READ, "DELETE FROM agents.grid"); // DANGER DANGER
  for(blk_i = super_node.begin(); blk_i != super_node.end(); blk_i++)
  {
    block = blk_i -> second;
    for(i = BLOCK_PADDING; i < BLOCK_PADDING + BLOCK_SIZE; i++)
    {
      for(j = BLOCK_PADDING; j < BLOCK_PADDING + BLOCK_SIZE; j++)
      {
        if(block -> map[i][j])
        {
          db_info -> insert_query_builder(block -> map[i][j]
                                         , i + block -> originx - BLOCK_PADDING
                                         , j + block -> originy - BLOCK_PADDING
                                         );
        }
      }
    }
  }
  db_info -> insert_query_builder(0, 0, 0);
  log -> record(ME, "Database updated");
}

string Game::user_want(int px1, int py1, int px2, int py2)
{
  if(px1 > px2)
  {
    int temp = px1;
    px1 = px2;
    px2 = temp;
  }
  if(py1 > py2)
  {
    int temp = px1;
    px1 = px2;
    px2 = temp;
  }
  string query = "SELECT * FROM agents.grid WHERE x>=" +
                  std::to_string(px1) + " AND x<=" + std::to_string(px2) +
                  " AND y>=" + std::to_string(py1) + " AND y<=" +
                  std::to_string(py2);
  log -> record(ME, query);
  crank_lock.lock();
  string result = db_info->run_query(EXPECT_CLIENT, query);
  crank_lock.unlock();
  return result;
}

int Game::user_does(int x, int y, CELL_TYPE t, CELL_TYPE user_type)
{
  if(crank_lock.try_lock())
  {
    uint64_t complessed_coord = compress_xy(x, y);
    if(user_type != DEAD_CELL)
    {
      change_buffer[complessed_coord] = t;
      log -> record("Move analyser", "failed check 0");
      crank_lock.unlock();
      return 0;
    }
    //FIRST check for type
    if(t != user_type || t != DEAD_CELL)
    {
      crank_lock.unlock();
      log -> record("Move analyser", "failed check 1");
      return 0;
    }
    //Second check
    Block *curr_block = get_curr_block(x, y);
    if(curr_block->map[curr_block->rectify_x(x)][curr_block->rectify_y(y)] + t != user_type)
    {
      crank_lock.unlock();
      log -> record("Move analyser", "failed check 2");
      return 0;
    }
    // Third check for n_neighbours
    if(!curr_block->can_place_here(t, curr_block->rectify_x(x), curr_block->rectify_y(y)))
    {
      crank_lock.unlock();
      log -> record("Move analyser", "failed check 3");
      return 0;
    }
    //Fourth check
    std::map<uint64_t, CELL_TYPE>::iterator i;
    i = change_buffer.find(complessed_coord);
    if(i != change_buffer.end())
    {
      change_buffer.erase(complessed_coord);
      change_buffer[complessed_coord] = DEAD_CELL;
      crank_lock.unlock();
      log -> record("Move analyser", "success: repeated");
      return 1;
    }
    change_buffer[complessed_coord] = t;
    crank_lock.unlock();
    log -> record("Move analyser", "success: simple");
    return 1; // success
  }
  return 0; // fail
}

void Game::load_from_db()
{
  int x, y, i, n;
  CELL_TYPE t;
  string res = db_info -> run_query(EXPECT_READ, "SELECT * FROM agents.grid");
  const char *point = res.c_str();
  n = stoi(string_get_next_token(point, STR_WHITE));
  point = string_seek(point, "\n"); // skip size;
  for(i = 0; i < n; i++)
  {
    x = stoi(string_get_next_token(point, STR_WHITE));
    point = string_seek(point, " ");
    y = stoi(string_get_next_token(point, STR_WHITE));
    point = string_seek(point, " ");
    t = stoi(string_get_next_token(point, STR_WHITE));
    point = string_seek(point, "\n");
    //TODO: place user_type
    CELL_TYPE user_type = 0;
    user_does(x, y, t, user_type); // impersonate user, since he does a good job anyway
  }
  flush_buf();
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
  delete action;
/* Reset everything, allowing the game to be recycled.
 */
  execution_lock.lock();
  flags = 0; // Important because it unsets running flag
}

void Game::ping_round()
{
  log -> record(ME, "Running pings on the slackers.");
  if(server)
  {
    server -> check_clients(9);
  }
  else
  {
    log -> record(ME, "No server to ping with.");
  }
}

string Game::getdets()
{
  return "";
}

void Game::demand_stat()
{
  log -> demand_stat();
  log -> record(ME, "Game is progressing quite nicely!");
  log -> record(ME, "We are currently at generation " + to_string(curr_gen));
  log -> record(ME, "Using " + to_string(super_node.size()) + " blocks");
  if(server)
  {
    log -> record(ME, "Here's my database report");
    db_info -> demand_stat();
  }
  else
  {
    log -> record(ME, "Running without a database!");
  }
  if(server)
  {
    server -> demand_stat();
  }
  else
  {
    log -> record(ME, "Running without a server!");
  }
}
