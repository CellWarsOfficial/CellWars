#ifndef BLOCK_HPP
#define BLOCK_HPP

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
  void *crank(void);
  void crank_cell(int x, int y);
  void revive_cell(int x, int y);
  int count_cell_neighbours(int x, int y);
  int valid_coordonate(int x, int y);
};

#endif
