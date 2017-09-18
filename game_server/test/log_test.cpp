#include "log_test.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>

using namespace std;

int fails = 0;
int tests = 0;
Logger *l;
#define F_NAME "log_test_aux1.l"
#define STD_TO_F_NAME "log_test_aux2.l"

void fast_fail()
{
  fprintf(stderr, "%d/%d tests failed, exiting prematurely", fails, tests);
  exit(fails);
}

void test_init()
{
  tests++;
  l = new Logger(100, 2000);
  if(l == 0)
  {
    fprintf(stderr, "Failed to create logger.\n");
    fails++;
    fast_fail();
  }
}

void test_file()
{
  tests++;
  if(l -> add_file((char*) F_NAME) == 0)
  {
    fprintf(stderr, "Failed to attach file to logger.\n");
    fails++;
  }
}

void test_record()
{
  tests++;
  if(l -> record("asd1") == 1)
  {
    fprintf(stderr, "Failed to record with 1 arg.\n");
    fails++;
  }
  tests++;
  if(l -> record("test1", "qwe1") == 1)
  {
    fprintf(stderr, "Failed to record with 2 args.\n");
    fails++;
  }
}

void test_mute()
{
  l -> mute();
  tests++;
  if(l -> record("asd2") == 0)
  {
    fprintf(stderr, "Failed to mute with 1 arg\n");
    fails++;
  }
  tests++;
  if(l -> record("test2", "asd2") == 0)
  {
    fprintf(stderr, "Failed to mute with 2 args\n");
    fails++;
  }
  l -> unmute();
  tests++;
  if(l -> record("asd3") == 1)
  {
    fprintf(stderr, "Failed to unmute with 1 arg.\n");
    fails++;
  }
  tests++;
  if(l -> record("test3", "qwe3") == 1)
  {
    fprintf(stderr, "Failed to unmute with 2 args.\n");
    fails++;
  }
}

void test_termination()
{
  delete l;
}

void check_file_output()
{
  tests++;
  ifstream to_check1(F_NAME);
  string line;
  int line_count = 0;
  while(getline(to_check1, line))
  {
    line_count++;
  }
  if(line_count != 6)
  {
    fprintf(stderr, "File does not contain correct number of lines.\n");
    fails++;
  }
  tests++;
  ifstream to_check2(STD_TO_F_NAME);
  line_count = 0;
  while(getline(to_check2, line))
  {
    line_count++;
  }
  if(line_count != 6)
  {
    fprintf(stderr, "STD output does not contain correct number of lines.\n");
    fails++;
  }
}

int main()
{
  remove(F_NAME);
  remove(STD_TO_F_NAME);
  if(freopen(STD_TO_F_NAME, "w", stdout) == NULL)
  {
    fprintf(stderr, "failed to initialise test, please retry\n");
    return -1;
  }
  test_init();
  test_file();
  test_record();
  test_mute();
  test_termination();
  fclose(stdout);
  check_file_output();
  fprintf(stderr, "%d/%d tests passed\n", tests - fails, tests);
  remove(F_NAME);
  remove(STD_TO_F_NAME);
  return fails;
}
