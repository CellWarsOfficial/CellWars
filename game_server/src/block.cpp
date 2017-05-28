#include <constants.hpp>
#include <block.hpp>
#include <malloc_safety.hpp>
#include <cstring>

/* class Block
 * {
 *   public:
 *   CELL_TYPE **map;
 *   int originx, originy;
 *   Block(int x, int y);
 *   Block(Block &other);
 *   ~Block();
 *   int get_x_relative(int relativity);
 *   int get_y_relative(int relativity);
 * };
 * block class defined in block.hpp, copied here for convenience. */

Block::Block(int x, int y)
{
  originx = x;
  originy = y;
  map = new CELL_TYPE*[BLOCK_SIZE];
  check_malloc(map);
  for(int i = 0; i < BLOCK_SIZE; i++)
  {
    map[i] = new CELL_TYPE[BLOCK_SIZE];
    check_malloc(map[i]);
  }
}

Block::Block(Block &other)
{
  originx = other.originx;
  originy = other.originy;
  map = new CELL_TYPE*[BLOCK_SIZE];
  check_malloc(map);
  for(int i = 0; i < BLOCK_SIZE; i++)
  {
    map[i] = new CELL_TYPE[BLOCK_SIZE];
    check_malloc(map[i]);
    memcpy(map[i], other.map[i], BLOCK_SIZE * sizeof(CELL_TYPE));
  }
}

Block::~Block()
{
  for(int i = 0; i < BLOCK_SIZE; i++)
  {
    delete[] map[i];
  }
  delete[] map;
}

int Block::get_x_relative(int relativity)
{
  return originx + relativity * BLOCK_SIZE;
}

int Block::get_y_relative(int relativity)
{
  return originy + relativity * BLOCK_SIZE;
}
