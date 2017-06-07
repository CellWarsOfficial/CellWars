#ifndef BLOCK_H
#define BLOCK_H

#include <bitset>
#include "constants.hpp"

class Block
{
  public:
  CELL_TYPE **map;
  int originx, originy;
  //std::bitset<BITMAP_SIZE> *bitmap;
  int *counts;
/* Constructor, copy constructor and destructor
 */
  Block(int x, int y);
  Block(Block &other);
  Block(Block *other);
  ~Block();
/* get_x_relative and get_y_relative functions are used to navigate block map
 */
  int get_x_relative(int relativity);
  int get_y_relative(int relativity);
  int rectify_x(int raw);
  int rectify_y(int raw);
};

/* 3 functions used for compressing 2 ints into a uint64_t.
 */

uint64_t compress_xy(int x, int y);

int get_x(uint64_t origin);

int get_y(uint64_t origin);

void dump(Block *b);

#endif
