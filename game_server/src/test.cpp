#include <math.hpp>
#include <cstdio>

int main ()
{
  unsigned long in = 97;
  std::string out = get_bin(in);
  printf("%s", out.c_str());
  return 0;
}
