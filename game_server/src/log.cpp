#include <log.hpp>
#include <constants.hpp>
#include <chrono>
#include <ctime>

#define ME "Logger"
#define UNKNOWN "Unknown"

Logger::Logger(int max_buf)
{
  buffer_size = max_buf;
  buffer_read = 0;
  buffer_write = 0;
  flags = 0;
  buffer = new string[max_buf];
  parser_thread = new thread(&Logger::parse_buffer, this);
}

Logger::~Logger()
{
  record(ME, "Log closed.");
  logger_lock.lock();
  flags = flags | JUST_28_MASK; // inform logger he should terminate
  logger_lock.unlock();
  parser_thread -> join();
  delete parser_thread;
  delete buffer;
  if(file)
  {
    fclose(file);
  }
}

void Logger::quiet()
{
  logger_lock.lock();
  flags = flags | JUST_30_MASK; // Log no longer prints to screen.
  logger_lock.unlock();
}

void Logger::loud()
{
  logger_lock.lock();
  flags = flags & NO_30_MASK; // Log prints to screen again.
  logger_lock.unlock();
}

void Logger::pause()
{
  logger_lock.lock();
  flags = flags & NO_31_MASK; // Log will pause after next print.
  logger_lock.unlock();
}

void Logger::unpause()
{
  logger_lock.lock();
  flags = flags | JUST_31_MASK; // Log will resume.
  logger_lock.unlock();
}

void Logger::mute()
{
  record(ME, "Skipping a few logs - mute.");
  logger_lock.lock();
  flags = flags | JUST_27_MASK; // Log will ignore record requests.
  logger_lock.unlock();
}

void Logger::unmute()
{
  logger_lock.lock();
  flags = flags & JUST_27_MASK; // Log will resume record requrests.
  logger_lock.unlock();
}

int Logger::add_file(char *f)
{
  logger_lock.lock();
  file = fopen(f, "a");
  logger_lock.unlock();
  return file != NULL;
}

int Logger::record(string message)
{
  if(LFLAG_terminate || LFLAG_mute)
  {
    return 1;
  }
  return record(UNKNOWN, message);
}

int Logger::record(string process, string message)
{
  if(LFLAG_terminate || LFLAG_mute)
  {
    return 1;
  }
  logger_lock.lock();
  buffer[buffer_read] = 
      '[' + to_string(time(NULL)) + ']' + process + ": " + message + '\n';
  buffer_read++;
  if(buffer_read == buffer_size)
  {
    buffer_read = 0;
    flags = flags | JUST_29_MASK; // Buffer wrapped around.
  }
  logger_lock.unlock();
  return 0;
}

void Logger::parse_buffer()
{
  while((!LFLAG_terminate) || (buffer_read != buffer_write))
  {
    if((buffer_read == buffer_write) || !LFLAG_running)
    {
      std::this_thread::sleep_for(std::chrono::seconds(LOG_BUFFER_DELAY));
      continue;
    }
    logger_lock.lock();
    while(buffer_read != buffer_write)
    {
      logger_lock.unlock();
      if(!LFLAG_quiet)
      {
        printf("%s", buffer[buffer_write].c_str());
      }
      if(file)
      {
        fprintf(file, "%s", buffer[buffer_write].c_str());
      }
      logger_lock.lock();
      buffer_write++;
      if(buffer_write == buffer_size)
      {
        buffer_write = 0;
        flags = flags & NO_29_MASK; // Buffer no longer wrapped.
      }
    }
    logger_lock.unlock();
  }
}
