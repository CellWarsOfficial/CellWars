#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <bitset>
#include "constants.hpp"

class Block
{
  public:
  CELL_TYPE **map;
  int originx, originy;
  std::bitset<BLOCK_SIZE * BLOCK_SIZE> bitmap;
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

#endif
