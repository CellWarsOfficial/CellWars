#include "constants.hpp"
#include "block.hpp"
#include "malloc_safety.hpp"
#include <cstring>

Block::Block(int x, int y)
{
  originx = x;
  originy = y;
  map = new CELL_TYPE*[BLOCK_FULL];
  check_malloc(map);
  for(int i = 0; i < BLOCK_FULL; i++)
  {
    map[i] = new CELL_TYPE[BLOCK_FULL]();
    check_malloc(map[i]);
    for(int j = 0; j < BLOCK_FULL; j++)
    {
      map[i][j] = DEAD_CELL;
    }
  }
  //bitmap = new std::bitset<BITMAP_SIZE>();
}

Block::Block(Block &other)
{
  originx = other.originx;
  originy = other.originy;
  map = new CELL_TYPE*[BLOCK_FULL];
  check_malloc(map);
  for(int i = 0; i < BLOCK_FULL; i++)
  {
    map[i] = new CELL_TYPE[BLOCK_FULL];
    check_malloc(map[i]);
    memcpy(map[i], other.map[i], BLOCK_FULL * sizeof(CELL_TYPE));
  }
}

Block::~Block()
{
  for(int i = 0; i < BLOCK_FULL; i++)
  {
    delete[] map[i];
  }
  delete[] map;
  //delete bitmap;
}

int Block::get_x_relative(int relativity)
{
  return originx + relativity * BLOCK_SIZE;
}

int Block::get_y_relative(int relativity)
{
  return originy + relativity * BLOCK_SIZE;
}

/* Rectify transforms absolute x and/or y into relative x/y.
 * Generally, it's absx/y - block.originx/y + BLOCK_PADDING
 */
int Block::rectify_x(int raw_x)
{
  return raw_x - originx + BLOCK_PADDING;
}

int Block::rectify_y(int raw_y)
{
  return raw_y - originy + BLOCK_PADDING;
}

uint64_t compress_xy(int x, int y)
{
  return (((uint64_t)((uint32_t) x)) << 32) | (uint64_t)((uint32_t) y);
}

int get_x(uint64_t origin)
{
  return (int)(origin >> 32);
}

int get_y(uint64_t origin)
{
  return (int)(origin & FULL_MASK);
}

/* Rectify transforms absolute x and/or y into relative x/y.
 * Generally, it's abs(x/y) - block.originx/y + BLOCK_PADDING
 * However, block.originx/y should always be a multiple of BLOCK_SIZE
 */

/* Block layout
 * <------------------BLOCK_FULL-------------------->
 * ##################################################)- BLOCK_PADDING
 * #! ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#
 * #? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#
 * #? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #   # := padding taken from
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#     adjacent blocks
 * #? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #     (no sync!)
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#   ? := Cell information
 * #? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #   ! := block position
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#     index
 * #? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#
 * #? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#
 * #? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#
 * #? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#
 * #? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#
 * #? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#
 * #? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? #
 * # ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ?#
 * ##################################################
 *  <-----------------BLOCK_SIZE------------------->
 */
