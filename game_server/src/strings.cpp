#include <strings.hpp>
#include <constants.hpp>
#include <math.hpp>
#include <cstdlib>
#include <ctime>

const char *skip_ws(const char *origin)
{
  if(origin == NULL)
  {
    return NULL;
  }
  int i = 0;
  for(; in(origin[i], STR_WHITE); i++);
  return origin + i;
}

bool in(char value, const char *list)
{
  if(list == NULL)
  {
    return false;
  }
  for(int i = 0; list[i]; i++)
  {
    if(value == list[i])
    {
      return true;
    }
  }
  return false;
}

const char *string_seek(const char *origin, const char *target)
{
  if((origin == NULL) || (target == NULL))
  {
    return NULL;
  }
  int i = 0, j;
  for(; origin[i]; i++)
  {
    for(j = i; origin[j];)
    {
      if(target[j - i] == origin[j])
      {
        j++;
      }
      else
      {
        break;
      }
      if(target[j - i] == 0)
      {
        return skip_ws(origin + j);
      }
    }
  }
  return NULL;
}

const char *string_seek(const char *origin, string target)
{
  return string_seek(origin, target.c_str());
}

string string_get_next_token(const char *origin, const char *seps)
{
  int i = 0;
  string result = "";
  if(origin == NULL)
  {
    return "";
  }
  for(; (origin[i] && !in(origin[i], seps)); i++)
  {
    result = result + origin[i];
  }
  return result;
}

void unmask(uint32_t *buf, uint32_t mask)
{
  int i = 0;
  for(; buf[i]; i++)
  {
    buf[i] = buf[i] & mask;
  }
}

void xormask(uint32_t *buf, uint32_t mask)
{
  int i = 0;
  for(; buf[i]; i++)
  {
    buf[i] = buf[i] ^ mask;
  }
}

void print_bytes(uint8_t *buf, int len)
{
  int i = 0;
  for(; i < len; i++)
  {
    printf("buf[%d] = %u\n", i, buf[i]);
  }
}

int str_eq(string s1, string s2)
{
  for(unsigned int i = 0; i < s1.length(); i++)
  {
    if(s1[i] != s2[i])
    {
      return 0;
    }
  }
  return 1;
}

int is_num(const char *s)
{
  if(s == NULL)
  {
    return 0;
  }
  int i = 0;
  if(s[i] == '-')
  {
    i++;
  }
  if((s[i]) && (in(s[i], STR_DIGITS)))
  {
    return 1;
  }
  return 0;
}

int is_num(string s)
{
  return is_num(s.c_str());
}

string get_random_string()
{
  return encode(to_string(rand()));
}

string wrap(string content)
{
  return wrap(content, "\"", "\"");
}

string wrap(string content, string seps)
{
  return wrap(content, seps, seps);
}

string wrap(string content, string sep_left, string sep_right)
{
  return sep_left + content + sep_right;
}

string form(string seq, string message, string sep)
{
  return seq + sep + message;
}

string form(int seq, string message, string sep)
{
  return form(to_string(seq), message,  sep);
}

string form(string seq, int message, string sep)
{
  return form(seq, to_string(message),  sep);
}

string form(int seq, int message, string sep)
{
  return form(to_string(seq), to_string(message),  sep);
}

string form(string seq, string message)
{
  return form(seq, message, ": ");
}

string form(int seq, string message)
{
  return form(to_string(seq), message);
}

string form(string seq, int message)
{
  return form(seq, to_string(message));
}

string form(int seq, int message)
{
  return form(to_string(seq), to_string(message));
}

string get_time_as_str()
{
  return to_string(time(0));
}

void check_not_null(const void *key)
{
  if(key == NULL)
  {
    throw invalid_argument("null");
  }
}

int get_seq_id(const char *source)
{
  return stoi(string_get_next_token(source, STR_WHITE));
}

string get_method(const char *source)
{
  return string_get_next_token(source, STR_WHITE);
}

string get_arg(const char *source, string target)
{
  return string_get_next_token(string_seek(source, target + "="), STR_WHITE);
}
