#include "constants.hpp"
#include "block.hpp"
#include "malloc_safety.hpp"
#include <cstring>

void dump(Block *b)
{
  for(int i = 0; i < BLOCK_FULL; i++)
  {
    for(int j = 0; j < BLOCK_FULL; j++)
    {
      printf("%d ", b->map[i][j]);
    }
    printf("\n");
  }
}

Block::Block(int x, int y)
{
  originx = x;
  originy = y;
  memset(border_changes
        , 0
        , BLOCK_NEIGHBOURING_REGIONS * sizeof(uint8_t)
        );
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
  counts = new int[(int) BLOCK_FULL / 10];
}

Block::Block(Block &other)
{
  originx = other.originx;
  originy = other.originy;
  memcpy(border_changes
        , other.border_changes
        , BLOCK_NEIGHBOURING_REGIONS * sizeof(uint8_t)
        );
  map = new CELL_TYPE*[BLOCK_FULL];
  check_malloc(map);
  for(int i = 0; i < BLOCK_FULL; i++)
  {
    map[i] = new CELL_TYPE[BLOCK_FULL];
    check_malloc(map[i]);
    memcpy(map[i], other.map[i], BLOCK_FULL * sizeof(CELL_TYPE));
  }
}

Block::Block(Block *other)
{
  originx = other -> originx;
  originy = other -> originy;
  memcpy(border_changes
        , other -> border_changes
        , BLOCK_NEIGHBOURING_REGIONS * sizeof(uint8_t)
        );
  map = new CELL_TYPE*[BLOCK_FULL];
  check_malloc(map);
  for(int i = 0; i < BLOCK_FULL; i++)
  {
    map[i] = new CELL_TYPE[BLOCK_FULL];
    check_malloc(map[i]);
    memcpy(map[i], other -> map[i], BLOCK_FULL * sizeof(CELL_TYPE));
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

uint64_t Block::get_xy_relative(int x_rel, int y_rel)
{
  return compress_xy(get_x_relative(x_rel), get_y_relative(y_rel));
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

void Block::set(int x, int y, CELL_TYPE cell)
{
  if(map[x][y] == cell)
  {
    return;
  }
  map[x][y] = cell;
  bool w = (x >= BLOCK_PADDING) && (x < 2 * BLOCK_PADDING);
  bool n = (y >= BLOCK_PADDING) && (y < 2 * BLOCK_PADDING);
/* BLOCK_SIZE and BLOCK_SIZE + BLOCK_PADDING are notation abuse, they should be
 * BLOCK_FULL - 2 * BLOCK_PADDING and BLOCK_FULL - BLOCK_PADDING, notation
 * similar to w and n. Notation abuse is a result of BLOCK_FULL's definition.
 */ 
  bool e = (x >= BLOCK_SIZE) && (x < BLOCK_SIZE + BLOCK_PADDING);
  bool s = (y >= BLOCK_SIZE) && (y < BLOCK_SIZE + BLOCK_PADDING);
  if(n)
  {
    border_changes[P_REGION_N] = 1;
    if(e)
    {
      border_changes[P_REGION_NE] = 1;
    }
    if(w)
    {
      border_changes[P_REGION_NW] = 1;
    }
  }
  if(s)
  {
    border_changes[P_REGION_S] = 1;
    if(e)
    {
      border_changes[P_REGION_SE] = 1;
    }
    if(w)
    {
      border_changes[P_REGION_SW] = 1;
    }
  }
  if(e)
  {
    border_changes[P_REGION_E] = 1;
  }
  if(w)
  {
    border_changes[P_REGION_W] = 1;
  }
}

/* sync_with modified this with values from other's region.
 */

void Block::sync_with(Block *other, int region)
{
  int i, j;
  if(region == P_REGION_NW)
  {
    for(i = 0; i < BLOCK_PADDING; i++)
    {
      for(j = 0; j < BLOCK_PADDING; j++)
      {
        this -> map[BLOCK_SIZE + i][BLOCK_SIZE + j] = other -> map[i][j];
      }
    }
  }
  if(region == P_REGION_N)
  {
    for(i = 0; i < BLOCK_PADDING; i++)
    {
      for(j = BLOCK_PADDING; j < BLOCK_SIZE + BLOCK_PADDING; j++)
      {
        this -> map[BLOCK_SIZE + i][j] = other -> map[i][j];
      }
    }
  }
  if(region == P_REGION_NE)
  {
    for(i = 0; i < BLOCK_PADDING; i++)
    {
      for(j = BLOCK_SIZE + BLOCK_PADDING; j < BLOCK_FULL; j++)
      {
        this -> map[BLOCK_SIZE + i][j - BLOCK_SIZE - BLOCK_PADDING] = other -> map[i][j];
      }
    }
  }
  if(region == P_REGION_W)
  {
    for(i = BLOCK_PADDING; i < BLOCK_SIZE + BLOCK_PADDING; i++)
    {
      for(j = 0; j < BLOCK_PADDING; j++)
      {
        this -> map[i][BLOCK_SIZE + j] = other -> map[i][j];
      }
    }
  }
  if(region == P_REGION_E)
  {
    for(i = BLOCK_PADDING; i < BLOCK_SIZE + BLOCK_PADDING; i++)
    {
      for(j = BLOCK_SIZE + BLOCK_PADDING; j < BLOCK_FULL; j++)
      {
        this -> map[i][j - BLOCK_SIZE - BLOCK_PADDING] = other -> map[i][j];
      }
    }
  }
  if(region == P_REGION_SW)
  {
    for(i = BLOCK_SIZE + BLOCK_PADDING; i < BLOCK_FULL; i++)
    {
      for(j = 0; j < BLOCK_PADDING; j++)
      {
        this -> map[i - BLOCK_SIZE - BLOCK_PADDING][BLOCK_SIZE + j] = other -> map[i][j];
      }
    }
  }
  if(region == P_REGION_S)
  {
    for(i = BLOCK_SIZE + BLOCK_PADDING; i < BLOCK_FULL; i++)
    {
      for(j = BLOCK_PADDING; j < BLOCK_SIZE + BLOCK_PADDING; j++)
      {
        this -> map[i - BLOCK_SIZE - BLOCK_PADDING][j] = other -> map[i][j];
      }
    }
  }
  if(region == P_REGION_E)
  {
    for(i = BLOCK_SIZE + BLOCK_PADDING; i < BLOCK_FULL; i++)
    {
      for(j = BLOCK_SIZE + BLOCK_PADDING; j < BLOCK_FULL; j++)
      {
        this -> map[i - BLOCK_SIZE - BLOCK_PADDING][j - BLOCK_SIZE - BLOCK_PADDING] = other -> map[i][j];
      }
    }
  }
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
