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
  action = new Crank();
  if(!GFLAG_nodb)
  {
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
  if(!GFLAG_nodb)
  {
    up_db();
  }
}

void Game::crank_stage(int generations)
{
  log -> record(ME, "Sending the change buffer");
  int x, y, o_x, o_y;
  CELL_TYPE t;
  Block *curr_block;
  std::map<uint64_t,Block*>::iterator find_res;
  while(!change_buffer.empty() && !block_queue.empty())
  {
    //db_info->run_query(NO_READ, (string) query_buffer.front());
    //query_buffer.pop();
    x = change_buffer.front();
    change_buffer.pop();
    y = change_buffer.front();
    change_buffer.pop();
    t = (CELL_TYPE) change_buffer.front();
    change_buffer.pop();
    o_x = change_buffer.front();
    change_buffer.pop();
    o_y = change_buffer.front();
    change_buffer.pop();
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
    curr_block->map[curr_block->rectify_x(x)][curr_block->rectify_y(y)] = t;
  }
  log -> record(ME, "Sent the change buffer");
  log -> record(ME, "Crank - start");
  std::map<uint64_t,Block*>::iterator i;
  for (i = super_node.begin(); i != super_node.end(); i++)
  {
    action->crank_for(i -> second, gen_to_run);
  }
  sync_padding();
  if(!GFLAG_nodb)
  {
    up_db();
  }
  log -> record(ME, "Crank - finish");
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
                           ,b -> get_x_relative(dy)
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
  for(; new_blocks; new_blocks = b -> duck)
  {
    b = (Block*) new_blocks;
    super_node[compress_xy(b -> originx, b -> originy)] = b;
  }
}

void Game::up_db()
{
  crank_lock.lock();
  log -> record(ME, "Updating database");
  std::map<uint64_t,Block*>::iterator i;
  for (i = super_node.begin(); i != super_node.end(); i++)
  {
    db_info -> update_db(i -> second);
  }
  log -> record(ME, "Database updated");
  crank_lock.unlock();
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
  String_container *c = (String_container *) db_info->run_query(EXPECT_CLIENT, query);
  crank_lock.unlock();
  string result = c -> s;
  free(c);
  return result;
}

int Game::user_does(int x, int y, CELL_TYPE t)
{
  crank_lock.lock();
  int updated_x = 0;
  int updated_y = 0;
  if(x < 0)
  {
    x = x * (-1);
    updated_x = 1;
  }
  if(y < 0)
  {
    y = y * (-1);
    updated_y = 1;
  }
  int o_x = x - x % 1000;
  int o_y = y - y % 1000;
  if(!(o_x % 1000) && updated_x)
  {
    o_x = o_x + 1000;
  }
  if(!(o_y % 1000) && updated_y)
  {
    o_y = o_y + 1000;
  }
  if(updated_x)
  {
    o_x = o_x * (-1);
  }
  if(updated_y)
  {
    o_y = o_y * (-1);
  }
  Block *curr_block;
  std::map<uint64_t,Block*>::iterator find_res = super_node.find(compress_xy(o_x, o_y));
  if(find_res != super_node.end())
  {
    curr_block = find_res->second;
  }
  else
  {
    curr_block = new Block(o_x, o_y);
    super_node[compress_xy(o_x, o_y)] = curr_block;
  }
  if(updated_x)
  {
    x = x * (-1);
  }
  if(updated_y)
  {
    y = y * (-1);
  }
  if(curr_block->map[curr_block->rectify_x(x)][curr_block->rectify_y(y)] != 0 && t)
  {
    crank_lock.unlock();
    return 1;
  }
  //curr_block->map[curr_block->rectify_x(x)][curr_block->rectify_y(y)] = t;
  change_buffer.push(x);
  change_buffer.push(y);
  change_buffer.push((int) t);
  change_buffer.push(o_x);
  change_buffer.push(o_y);
  //string query = "DELETE FROM agents.grid WHERE x=" +
  //               std::to_string(x) + " AND y=" + std::to_string(y);
  //db_info->run_query(NO_READ, query);
  //query_buffer.push(query);
  //if(t)
  //{
  //  query = "INSERT INTO agents.grid(user_id, x, y) VALUES (" +
  //          std::to_string(t) + ", " + std::to_string(x) + ", " +
  //          std::to_string(y) + ")";
  //  //db_info->run_query(NO_READ, query);
  //  query_buffer.push(query);
  //}
  crank_lock.unlock();
  return 2;
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
