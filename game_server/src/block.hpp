#ifndef BLOCK_H
#define BLOCK_H

#include <constants.hpp>

class Block
{
  public:
  CELL_TYPE **map;
  int originx, originy;
/* Constructor, copy constructor and destructor
 */
  Block(int x, int y);
  Block(Block &other);
  ~Block();
/* get_x_relative and get_y_relative functions are used to navigate block map
 */
  int get_x_relative(int relativity);
  int get_y_relative(int relativity);
};

/* 3 functions used for compressing 2 ints into a long.
 */

long compress_xy(int x, int y);

int get_x(long origin);

int get_y(long origin);

#endif
