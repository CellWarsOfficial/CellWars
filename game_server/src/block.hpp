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
/* Constructor, copy constructor and destructor
 */
  Block(int x, int y);
  Block(Block &other);
  ~Block();
/* get_x_relative and get_y_relative functions are used to navigate block map
 */
  int get_x_relative(int relativity);
  int get_y_relative(int relativity);
  int rectify_x(int raw);
  int rectify_y(int raw);
};

/* 3 functions used for compressing 2 ints into a long.
 */

long compress_xy(int x, int y);

int get_x(long origin);

int get_y(long origin);

#endif
