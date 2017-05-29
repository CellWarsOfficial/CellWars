#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <mutex>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <constants.hpp>

#define LFLAG_running (flags&JUST_31_MASK)
#define LFLAG_quiet (flags&JUST_30_MASK)
#define LFLAG_wrap (flags&JUST_29_MASK)
#define LFLAG_terminate (flags&JUST_28_MASK)
#define LFLAG_mute (flags&JUST_27_MASK)

using namespace std;

class Logger
{
  public:
  Logger(int max_buf);
  ~Logger();
  void quiet();
  void loud();
  void pause();
  void unpause();
  void mute();
  void unmute();
  int add_file(char *f);
  int record(string message);
  int record(string process, string message);
  private:
  void parse_buffer();
  mutex logger_lock;
  string *buffer;
  int buffer_size;
  int buffer_read;
  int buffer_write;
  FILE *file;
  uint32_t flags;
  thread *parser_thread;
};

#endif
