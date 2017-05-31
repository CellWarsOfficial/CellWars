#include "malloc_safety.hpp"
#include <cstdio>
#include <cstdlib>

bool use_safety = 1;

void check_malloc(void* pointer)
{
  if(use_safety)
  {
    if(pointer == NULL)
    {
      fprintf(stderr, "Malloc failure\n");
      exit(EXIT_FAILURE);
    }
  }
}
