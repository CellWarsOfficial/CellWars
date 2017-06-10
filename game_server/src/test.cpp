#include <math.hpp>
#include <cstdio>

int main ()
{
  string in = "Man is distinguished, not only by his reason, but by this \
               singular passion from other animals, which is a lust of the \
               mind, that by a perseverance of delight in the continued \
               and indefatigable generation of knowledge, exceeds the short \
               vehemence of any carnal pleasure.";
  printf("%s\n", encode_base64(in).c_str());
  return 0;
}
