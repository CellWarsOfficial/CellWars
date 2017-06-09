#include <math.hpp>

string encode(string input)
{
  input = input + FIXED_STRING;
  std::hash<std::string> str_hash;
  unsigned long hash = str_hash(input);
  string hash_string = num_to_str(hash);
  int i = 0;
  string long_binary = "";
  while(i < hash_string.length())
  {
     long_binary = long_binary + num_to_binary(hash_string[i]);
     i++;
  }
  long_binary = "10011010110000101101110";
  string encoded = "";
  string buff;
  int idx = 0;
  int j = 0;
  while(j < long_binary.length())
  {
    idx = encode_six_bits(long_binary, j);
    printf("%i\n", idx);
    buff = buff + BASE_64_TABLE[idx];
    j = j + 6;
  }
  printf("%s\n", long_binary.c_str());
  return buff;
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

int encode_six_bits(string input, int index)
{
  int k = 0;
  string buff;
  while(k < 5)
  {
    buff = buff + input[index + k];
    k++;
  }
  return binary_to_num(buff);
}

int binary_to_num(string input)
{
  int result;
  int i = 0;
  for (result = 0; i <= input.length(); i++)
  {
    printf("HERE\n");
    if (input[i] == '1')
      result = (result << 1) | 1;
    else if (input[i] == '0' )
      result <<= 1;
  }
  return result;
}

int str_to_num(string input)
{
  int result = 0;
  for(int i = 0; input.length(); i++)
  {
    result = result + ((int) input[input.length() - i - 1]) * std::pow(10, i);
  }
  return result;
}
