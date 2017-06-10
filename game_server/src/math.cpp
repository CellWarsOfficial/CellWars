#include <math.hpp>
#include <openssl/sha.h>
#include <sstream>
#include <iostream>

string encode(string input)
{
  input = input + FIXED_STRING;
  size_t len = input.length();
  unsigned char hash[20];
  SHA1(reinterpret_cast<unsigned char*>(&input), len, hash);
  stringstream s;
  s << hash;
  string hashed_out = s.str();
  string output = encode_base64(hashed_out);
  return output;
}

unsigned char* convert_type(string input)
{
  size_t len = input.length();
  unsigned char* output = (unsigned char*) malloc(len * sizeof(char)); 
  check_malloc(output);
  return output;
}

string encode_base64(string input)
{
  int i = 0;
  string long_binary = "";
  while(i < (int) input.length())
  {
     long_binary = long_binary + num_to_binary(((int) input[i]));
     i++;
  }
  int padding_no = 0;
  string pad = "00000000";
  while((long_binary.length() % 3) != 0)
  {
    long_binary = long_binary + pad; 
    padding_no++;
  }
  string encoded = "";
  string buff;
  int idx = 0;
  int j = 0;
  while(j < (int) long_binary.length())
  {
    idx = encode_six_bits(long_binary, j);
    buff = buff + BASE_64_TABLE[idx];
    j = j + 6;
  }
  for(int i = 0; i < padding_no; i++)
  {
    buff[buff.length() - 1 - i] = '=';
  }
  return buff;
}

string add_padding(string input)
{
  
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
  int k = 8 - output.length();
  for(int i = 0; i < k; i++)
  {
    output = "0" + output;
  }
  return output;
}

int encode_six_bits(string input, int index)
{
  int k = 0;
  string buff = "";
  while(k < 6)
  {
    buff = buff + input[index + k];
    k = k + 1;
  }
  return binary_to_num(buff);
}

int binary_to_num(string input)
{
  int result;
  int i = 0;
  for (result = 0; i < (int) input.length(); i++)
  {
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
