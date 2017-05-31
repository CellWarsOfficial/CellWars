#include <all.hpp>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <thread>
#include <cstdlib>

#define ME "Init"

using namespace std;

bool use_safety = 1;

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
  void *db_info = NULL;
  FLAG_TYPE flags = 0;
  int gtc = DEFAULT_GTC;
  int wait_time = DEFAULT_WAIT_TIME;
  Logger *log = new Logger(LOG_MAX_BUFFER_DEFAULT, LOG_BUFFER_DELAY_DEFAULT);
/* Parsing program arguments
 */
  int i;
  for(i = 1; i < argc; i++)
  {
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
      db_info = init_db(argv[i], log);
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
    if(equ(argv[i], "-wait", "-w"))
    {
      i++;
      check_limit(i, argc);
      wait_time = atoi(argv[i]);
      if(wait_time <= 0)
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
    if(equ(argv[i], "-log"))
    {
      i++;
      check_limit(i, argc);
      log -> add_file(argv[i]);
      continue;
    }
    fprintf(stderr, "Unrecognised argument \"%s\"\nIgnoring.\n", argv[i]);
  }
  if(db_info == NULL)
  {
    fprintf(stderr, "Initialisation failure, missing database.\n");
    exit(EXIT_FAILURE);
  }
  game = new Game(db_info, log);
  init_server_ui(log);
  thread *game_thread = new thread(&Game::start, game, flags, gtc, wait_time);

  /* main terminating kills process.
   */
  game_thread -> join();
  delete log;
  delete game;
  return 0;
}
