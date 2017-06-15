#ifndef MATH_H
#define MATH_H

#include<functional>
#include<string>
#include<cstring>
#include<constants.hpp>
#include<all.hpp>
#include <cmath>

string encode(string input);
string encode_base64(unsigned char *input, int length);
string num_to_binary(unsigned long input);
int encode_six_bits(string input, int index);
int binary_to_num(string input);

#endif
