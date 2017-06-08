#include <math.hpp>

string encode(string input)
{
  input = input + FIXED_STRING; 
  std::hash<std::string> str_hash;
  //printf("%i\n", str_hash(input));
  unsigned long int hash = str_hash(input); 

}

string get_bin(unsigned long input)
{
  std::string output;
  int q;
  while(input > 0)
  {
    q = input % 10;
    output = q + output;
    input = input / 10;
  }
  return output;

}
