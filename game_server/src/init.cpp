#include <all.hpp>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <thread>
#include <cstdlib>
#include <ctime>

#define ME "Init"

using namespace std;

/* Equ returns true if s1 is equal to s2(or s3 if provided).
 */

int equ(char *s1, string s2)
{
  int i = 0;
  for(; (s1[i] != 0) && (s2[i] != 0) && (s1[i] == s2[i]); i++);
  return s1[i] == s2[i];
}

int equ(char *s1, string s2, string s3)
{
  return equ(s1, s2) || equ(s1, s3);
}

void check_limit(int i, int l)
{
  if(i == l)
  {
    fprintf(stderr, "Expected argument missing.\n");
    exit(EXIT_FAILURE);
  }
}

void* misc(uint32_t f, int gtc, int w)
{
  return NULL;
}

int main(int argc, char **argv)
{
/* Initialising default information
 */
  Logger *log = new Logger(LOG_MAX_BUFFER_DEFAULT, LOG_BUFFER_DELAY_DEFAULT);
  DB_conn *db_info = NULL;
  Player_Manager *player_manager = NULL;
  Server *server = NULL;
  Game *game = NULL;
  thread *game_thread = NULL;
  thread *server_thread = NULL;
  string db_pass = "";

  FLAG_TYPE flags = 0;
  int gtc = DEFAULT_GTC;
  int server_p = SV_DEF_PORT;
  int wait_time = DEFAULT_WAIT_TIME;
  srand(time(0));
/* Parsing program arguments
 */
  int i;
  for(i = 1; i < argc; i++)
  {
    if(equ(argv[i], "-hijack"))
    {
      hijack(log);
      continue;
    }
    if(equ(argv[i], "-exit"))
    {
      goto cleanup;
    }
    if(equ(argv[i], "-load"))
    {
      i++;
      check_limit(i, argc);
      int no_files = std::stoi(argv[i]);
      if(db_info == 0)
      {
        fprintf(stderr, "Database missing when loading.\n");
        exit(EXIT_FAILURE);
      }
      for(int k = 1; k <= no_files; k++)
      {
        if(equ(argv[i + k], "-o"))
        {
          i++;
          check_limit(i + k, argc);
          // int ofx = stoi(argv[i + k]);
          i++;
          check_limit(i+k, argc);
          // int ofy = stoi(argv[i + k]);
          i++;
          check_limit(i+k, argc);
          // db_info->rewrite_db(argv[i + k], ofx, ofy);
        }
        else
        {
          check_limit(i + k, argc);
          // db_info->rewrite_db(argv[k + i]);
        }
      }
      i += no_files;
      continue;
    }
    if(equ(argv[i], "-no_safe"))
    {
      use_safety = 0;
      log -> record(ME, "Removing safety.");
      continue;
    }
    if(equ(argv[i], "-debug_step"))
    {
      flags = flags | JUST_30_MASK; // set stepped_debug flag
      log -> record(ME, "Using debug steps.");
      continue;
    }
    if(equ(argv[i], "-generations", "-gtc"))
    {
      i++;
      check_limit(i, argc);
      gtc = atoi(argv[i]);
      if((gtc <= 0) || (gtc > BLOCK_PADDING))
      {
        fprintf(stderr, "Generations requirement unreasonable. Defaulting.\n");
        gtc = DEFAULT_GTC;
      }
      continue;
    }
    if(equ(argv[i], "-http_port"))
    {
      i++;
      check_limit(i, argc);
      server_p = atoi(argv[i]);
      if((server_p <= 0) || (server_p > SV_MAX_PORT))
      {
        fprintf(stderr, "Inappropiate server port. Defaulting.\n");
        server_p = SV_DEF_PORT;
      }
      continue;
    }
    if(equ(argv[i], "-pass"))
    {
      i++;
      check_limit(i, argc);
      db_pass = argv[i];
      continue;
    }
    if(equ(argv[i], "-wait", "-w"))
    {
      i++;
      check_limit(i, argc);
      wait_time = atoi(argv[i]);
      if(wait_time < 0)
      {
        fprintf(stderr, "Wait time unreasonable. Defaulting.\n");
        wait_time = DEFAULT_WAIT_TIME;
      }
      continue;
    }
    if(equ(argv[i], "-quiet", "-q"))
    {
      log -> quiet();
      continue;
    }
    if(equ(argv[i], "-log", "-l"))
    {
      i++;
      check_limit(i, argc);
      log -> add_file(argv[i]);
      continue;
    }
    fprintf(stderr, "Unrecognised argument \"%s\"\nIgnoring.\n", argv[i]);
  }
  db_info = new DB_conn(log);
  db_info -> set_variable(DB_PASS_VAR, db_pass);
  player_manager = new Player_Manager(db_info, log);
  server = new Server(server_p, db_info, player_manager, log);
  game = new Game(db_info, player_manager, log);
  game_thread = new thread(&Game::start, game, flags, gtc, wait_time, 25);
  server_thread = new thread(&Server::start, server);
  init_server_ui(log, game, server);

  /* main terminating kills process.
   */
  game_thread -> join();
  delete game_thread;
cleanup:
  if(server && server_thread)
  {
    server -> kill();
    server_thread -> join();
    delete server;
    delete server_thread;
  }
  if(player_manager)
  {
    delete player_manager;
  }
  if(db_info)
  {
    delete db_info;
  }
  if(game)
  {
    delete game;
  }
  if(log)
  {
    delete log;
  }
  return 0;
}
