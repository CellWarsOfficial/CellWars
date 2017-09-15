#ifndef STRINGS_HPP
#define STRINGS_HPP

#include <string>
#include <cstring>

using namespace std;

bool in(char value, const char *list);

const char *skip_ws(const char *origin);

const char *string_seek(const char *origin, const char *target);

const char *string_seek(const char *origin, string target);

string string_get_next_token(const char *origin, const char *seps);

void unmask(uint32_t *buf, uint32_t mask);

void xormask(uint32_t *buf, uint32_t mask);

void print_bytes(uint8_t *buf, int len);

int str_eq(string s1, string s2);

int is_num(const char *s);

int is_num(string s);

string get_random_string();

string form(string seq, string message, string sep);

string form(int seq, string message, string sep);

string form(string seq, int message, string sep);

string form(int seq, int message, string sep);

string form(string seq, string message);

string form(int seq, string message);

string form(string seq, int message);

string form(int seq, int message);

string get_time_as_str();

#endif
