#ifndef BLOCK_H
#define BLOCK_H

#include <bitset>
#include "constants.hpp"

class Block
{
  public:
  CELL_TYPE **map;
  int originx, originy;
  uint8_t border_changes[8];
  //std::bitset<BITMAP_SIZE> *bitmap;
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
  uint64_t get_xy_relative(int x_rel, int y_rel);
  int rectify_x(int raw);
  int rectify_y(int raw);
  void set(int x, int y, CELL_TYPE cell);
  void sync_with(Block *other, int region);
  void *duck;
};

/* 3 functions used for compressing 2 ints into a uint64_t.
 */

int valid_coordonate(int x, int y);

uint64_t compress_xy(int x, int y);

int get_x(uint64_t origin);

int get_y(uint64_t origin);

void dump(Block *b);

int find_block_origin(int position);

#endif
