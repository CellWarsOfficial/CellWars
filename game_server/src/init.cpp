#include <all.hpp>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>

using namespace std;

bool use_safety = 1;

int equ(char *s1, string s2)
{
  int i = 0;
  for(; (s1[i] != 0) && (s2[i] != 0) && (s1[i] == s2[i]); i++);
  return s1[i] == s2[i];
}

void check_limit(int i, int l)
{
  if(i == l)
  {
    fprintf(stderr, "Expected argument missing.\n");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char **argv)
{
  db_info = NULL;
  int i;
  for(i = 1; i < argc; i++)
  {
    if(equ(argv[i], "-no_safe"))
    {
      use_safety = 0;
      continue;
    }
    if(equ(argv[i], "-db"))
    {
      i++;
      check_limit(i, argc);
      init_db(argv[i]);
      continue;
    }
    fprintf(stderr, "Unrecognised argument \"%s\"\nIgnoring.\n", argv[i]);
  }
  if(db_info == NULL)
  {
    fprintf(stderr, "Initialisation failure, missing database.\n");
    exit(EXIT_FAILURE);
  }
}
