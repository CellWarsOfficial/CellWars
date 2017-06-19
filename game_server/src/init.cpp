#include "all.hpp"
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <thread>
#include <cstdlib>

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
  DB_conn *db_info = NULL;
  DB_conn *db_info2 = NULL;
  thread *game_thread = NULL;
  Server *server = NULL;
  FLAG_TYPE flags = 0;
  int gtc = DEFAULT_GTC;
  int server_p = SV_DEF_PORT;
  int wait_time = DEFAULT_WAIT_TIME;
  Logger *log = new Logger(LOG_MAX_BUFFER_DEFAULT, LOG_BUFFER_DELAY_DEFAULT);
/* Parsing program arguments
 */
  int i;
  for(i = 1; i < argc; i++)
  {
    if(equ(argv[i], "-exit"))
    {
      goto cleanup;
    }
    if(equ(argv[i], "-load"))
    {
      i++;
      check_limit(i, argc);
      int no_files = std::stoi(argv[i]);
      // i++;
      // check_limit(i, argc);
      if(db_info == 0)
      {
        fprintf(stderr, "Database missing when loading.\n");
        exit(EXIT_FAILURE);
      }
      db_info->clean_db();
      for(int k = 1; k < no_files+1; k++)
      {
        if(equ(argv[i + k], "-o"))
        {
          i++;
          check_limit(i+k, argc);
          printf("HERE1 x: %s\n", argv[i+k]);
          int ofx = stoi(argv[i+k]);
          i++;
          check_limit(i+k, argc);
          printf("HERE1 y: %s\n", argv[i+k]);
          int ofy = stoi(argv[i+k]);
          i++;
          check_limit(i+k, argc);
          db_info->rewrite_db(argv[i+k], ofx, ofy);
        }
        else
        {
          check_limit(i + k, argc);
          db_info->rewrite_db(argv[k + i]);
        }
      }
      i+=no_files;
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
    if(equ(argv[i], "-db"))
    {
      i++;
      check_limit(i, argc);
      db_info = new DB_conn(argv[i], log);
      db_info2 = new DB_conn(argv[i], log);
      if((db_info == 0) || !(db_info -> safe)
        || (db_info2 == 0) || !(db_info2 -> safe))
      {
        fprintf(stderr, "Database failure. Exiting\n");
        exit(EXIT_FAILURE);
      }
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
    if(equ(argv[i], "-fast_kill", "-fk"))
    {
      flags = flags | JUST_26_MASK; // set no server flag
      flags = flags | JUST_27_MASK; // set no database flag
      continue;
    }
    if(equ(argv[i], "-no_db", "-nd"))
    {
      flags = flags | JUST_27_MASK; // set no database flag
      continue;
    }
    if(equ(argv[i], "-no_server", "-ns"))
    {
      flags = flags | JUST_26_MASK; // set no server flag
      continue;
    }
    fprintf(stderr, "Unrecognised argument \"%s\"\nIgnoring.\n", argv[i]);
  }
  if(db_info == NULL)
  {
    fprintf(stderr, "Initialisation failure, missing database.\n");
    exit(EXIT_FAILURE);
  }
  if(!GFLAG_nosv)
  {
    server = new Server(server_p, db_info2, log);
  }
  game = new Game(db_info, server, log);
  game_thread = new thread(&Game::start, game, flags, gtc, wait_time);
  if(!GFLAG_nosv)
  {
    new thread(&Server::start, server, game);
  }
  init_server_ui(log);

  /* main terminating kills process.
   */
  game_thread -> join();
cleanup:
  if(server)
  {
    delete server;
  }
  if(db_info2)
  {
    delete db_info2;
  }
  if(game)
  {
    delete game;
  }
  if(db_info)
  {
    delete db_info;
  }
  if(log)
  {
    delete log;
  }
  return 0;
}
