#include <math.hpp>
#include <openssl/sha.h>

/*
string num_to_str(unsigned long input)
{
  std::string output = "";
  char q;
  while(input > 0)
  {
    q = (char) (input % 10 + 48);
    output = q + output;
    input = input / 10;
  }
  return output;
}
*/

string sha1_to_binary(unsigned char* input)
{
  string output = "";
  int i;
  unsigned char mask;
  for(i = 0; i < SHA_DIGEST_LENGTH; i++)
  {
    for(mask = 128; mask; mask = mask >> 1)
    {
      if(input[i] & mask)
      {
        output += '1';
      }
      else
      {
        output += '0';
      }
    }
  }
  return output;
}

/*
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
*/

int binary_to_num(string input)
{
  int result;
  unsigned int i = 0;
  for (result = 0; i < input.length(); i++)
  {
    if (input[i] == '1')
      result = (result << 1) | 1;
    else if (input[i] == '0' )
      result <<= 1;
  }
  return result;
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

/*
int str_to_num(string input)
{
  int result = 0;
  for(int i = 0; input.length(); i++)
  {
    result = result + ((int) input[input.length() - i - 1]) * std::pow(10, i);
  }
  return result;
}
*/

string encode(string input)
{
  input = input + FIXED_STRING;
//  std::hash<std::string> str_hash;
  printf("%s\n", input.c_str());
//  unsigned long hash = str_hash(input);
  unsigned char true_hash[SHA_DIGEST_LENGTH];
  SHA1((const unsigned char*)input.c_str(), sizeof(input) - 1, true_hash);
  printf("%s\n", true_hash);
//  string hash_string = num_to_str(hash);
//  unsigned int i = 0;
  string long_binary = sha1_to_binary(true_hash);
/*
  while(i < hash_string.length())
  {
     long_binary = long_binary + num_to_binary(hash_string[i]);
     i++;
  }
*/
  string encoded = "";
  string buff;
  int idx = 0;
  unsigned int j = 0;
  while(j < long_binary.length())
  {
    idx = encode_six_bits(long_binary, j);
    buff = buff + BASE_64_TABLE[idx];
    j = j + 6;
  }
  return buff;
}
