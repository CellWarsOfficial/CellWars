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

void *Block::crank(void)
{
  for(int i = 0; i < BLOCK_SIZE; i++)
  {
    for(int j = 0; j < BLOCK_SIZE; j++)
    {
      crank_cell(i, j);
    }
  }
  return map;
}

int Block::valid_coordonate(int x, int y)
  {
    return (0 <= x && x < BLOCK_SIZE) && (0 <= y && y < BLOCK_SIZE); 
  }

int Block::count_cell_neighbours(int x, int y)
{
  int n = 0;
  for(int i = -1; i < 2; i++)
  {
    for(int j = -1; j < 2; j++)
    {
      if(valid_coordonate(x + i, y + j) && i != 0 && j != 0 && map[x + i][y + j] != DEAD_CELL)
      {
        n++;
      }
    }
  }
  return n;
}

void Block::revive_cell(int x, int y)
{
  CELL_TYPE types[3];
  int h = 0;
  for(int i = -1; i < 2; i++)
  {
    for(int j = -1; j < 2; j++)
    {
      if(valid_coordonate(x + i, y + j) && i != 0 && j != 0 && map[x + i][y + j] != DEAD_CELL)
      {
        for (int m = 0; m < 3; m++)
        {
          if(map[x + i][y + j] == types[m])
          {
            map[x][y] = types[m];
          }
        }
        types[h] = map[x + i][y + j];
      }
    }
  }
}

void Block::crank_cell(int x, int y)
{
  int n_neighbours = count_cell_neighbours(x, y);
  if(n_neighbours < 2 && map[x][y] != DEAD_CELL)
  {
    map[x][y] = DEAD_CELL;
  } 
  else if(n_neighbours == 3 && map[x][y] == DEAD_CELL)
  {
    revive_cell(x, y);
  }
  else if (n_neighbours > 3 && map[x][y] != DEAD_CELL)
  {
    map[x][y] = DEAD_CELL; 
  }
  else
  {
    return;
  }
}

int Block::equals(Block &other)
{
  for (int i = 0; i < BLOCK_SIZE; i++)
  {
    for (int j = 0; j < BLOCK_SIZE; j++)
    {
      if(map[i][j] != other.map[i][j])
      {
        return 0;
      }
    }
  }
  return 1;
}
