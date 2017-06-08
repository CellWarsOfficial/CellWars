#include <math.hpp>
#include <cstdio>

int main ()
{
  long unsigned int in = 97;
  std::string out = get_binary(in);
  printf("%s", out);
  return 0;
}
