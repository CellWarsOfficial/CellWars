#include <strings.hpp>
#include <constants.hpp>

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
