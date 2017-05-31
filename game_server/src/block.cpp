#include "constants.hpp"
#include "block.hpp"
#include "malloc_safety.hpp"
#include <cstring>

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
    for(int j = 0; j < BLOCK_SIZE; j++)
    {
      map[i][j] = DEAD_CELL;
    }
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
  delete bitmap;
}

int Block::get_x_relative(int relativity)
{
  return originx + relativity * BLOCK_SIZE;
}

int Block::get_y_relative(int relativity)
{
  return originy + relativity * BLOCK_SIZE;
}
