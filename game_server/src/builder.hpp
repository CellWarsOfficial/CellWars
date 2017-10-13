#ifndef BUILDER_HPP
#define BUILDER_HPP

#include <string>
#include <mutex>
#include <constants.hpp>

using namespace std;

class Query_builder
{
  public:
  Query_builder();
  ~Query_builder();
  int add(int x, int y, CELL_TYPE t);
  string get();
  private:
  int size;
  string acc;
  mutex builder_lock;
};

#endif
