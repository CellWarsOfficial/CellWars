#include <math.hpp>

string encode(string input)
{
  input = input + FIXED_STRING;
  std::hash<std::string> str_hash;
  //printf("%i\n", str_hash(input));
  unsigned long hash = str_hash(input);
  string hash_string = num_to_str(hash);
  int i = 0;
  string long_binary = "";
  while(hash_string[i] != '\0')
  {
     long_binary = long_binary + num_to_binary(hash_string[i]);
     i++;
  }


  return 0;
}

string num_to_str(unsigned long input)
{
  std::string output;
  char q;
  while(input > 0)
  {
    q = (char) (input % 10 + 48);
    output = q + output;
    input = input / 10;
  }
  return output;
}

string str_to_binary(string input)
{


  return 0;
}

string num_to_binary(unsigned long input)
{
  string output = "";
  while(input > 0)
  {
    output =((char) ((input % 2) + 48)) + output;
    input = input / 2;
  }
  return output;
}

string encode_six_bits(string input, int index)
{
  int k = 0;
  string buff;
  while(k < 5)
  {
    buff = buff + input[index + k]; 
  }
  return binary_to_num(buff);
}

int binary_to_num(string input)
{
  int result;
  int i = 0;
  for (result = 0; input[i] != '\0'; i++) {
    if (input[i] == '1')
      result = (result << 1) | 1;
    else if (input[i] == '0' )
      result <<= 1;
    else
      break;
    }
    return result;
}


