#include "all.hpp"
#include <cstdio>
#include <cstring>
#include <string>

using namespace std;

int equ(char *s1, string s2)
{
  int i = 0;
  for(; (s1[i] != 0) && (s2[i] != 0) && (s1[i] == s2[i]); i++);
  return s1[i] == s2[i];
}

bool use_safety = 1;

int main(int argc, char **argv)
{
#if defined(CRANK_TEST_HPP)
  run_tests();
#elif !defined(CRANK_TEST_HPP)
  int i;
  for(i = 1; i < argc; i++)
  {
    if(equ(argv[i], "-no_safe"))
    {
      use_safety = 0;
      continue;
    }
    fprintf(stderr, "Unrecognised argument \"%s\"\nIgnoring.\n", argv[i]);
  }
#endif
}

