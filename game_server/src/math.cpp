#include <math.hpp>

string encode(string input)
{
  input = input + FIXED_STRING; 
  std::hash<std::string> str_hash;
  //printf("%i\n", str_hash(input));
  long unsigned int hash = str_hash(input); 

}

string get_binary(long unsigned int input)
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
