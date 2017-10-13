#include <builder.hpp>
#include <strings.hpp>
#include <constants.hpp>

Query_builder::Query_builder()
{
  size = 0;
  acc = "";
}

Query_builder::~Query_builder()
{
}

int Query_builder::add(int x, int y, CELL_TYPE t)
{
  int warn = 0;
  builder_lock.lock();
  if(size)
  {
    acc = acc + ",";
    warn = size > DB_MAX_BUILD_SIZE;
  }
  size++;
  acc = acc + wrap(
        form(wrap("x"), x)
          + "," + form(wrap("y"), y)
          + "," + form(wrap("t"), t)
        , "{", "}");
  builder_lock.unlock();
  return warn;
}

string Query_builder::get()
{
  string ret;
  builder_lock.lock();
  ret = wrap(acc, "[", "]");
  acc = "";
  size = 0;
  builder_lock.unlock();
  return ret;
}
