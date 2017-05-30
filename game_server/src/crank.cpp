#include <crank.hpp>
#include <constants.hpp>
#include <block.hpp>

Block &crank(Block &block)
{
  for(int i = 0; i < BLOCK_SIZE; i++)
  {
    for(int j = 0; j < BLOCK_SIZE; j++)
    {
      crank_cell(block, i, j);
    }
  }
  return block;
}

int valid_coordonate(int x, int y)
  {
    return (0 <= x && x < BLOCK_SIZE) && (0 <= y && y < BLOCK_SIZE);
  }

int count_cell_neighbours(Block &block, int x, int y)
{
  int n = 0;
  for(int i = -1; i < 2; i++)
  {
    for(int j = -1; j < 2; j++)
    {
      if(valid_coordonate(x + i, y + j) && i != 0 && j != 0 && block.map[x + i][y + j] != DEAD_CELL)
      {
        n++;
      }
    }
  }
  return n;
}

void revive_cell(Block &block, int x, int y)
{
  CELL_TYPE types[3];
  int h = 0;
  for(int i = -1; i < 2; i++)
  {
    for(int j = -1; j < 2; j++)
    {
      if(valid_coordonate(x + i, y + j) && i != 0 && j != 0 && block.map[x + i][y + j] != DEAD_CELL)
      {
        for (int m = 0; m < 3; m++)
        {
          if(block.map[x + i][y + j] == types[m])
          {
            block.map[x][y] = types[m];
          }
        }
        types[h] = block.map[x + i][y + j];
      }
    }
  }
}

void crank_cell(Block &block, int x, int y)
{
  int n_neighbours = count_cell_neighbours(block, x, y);
  if(n_neighbours < 2 && block.map[x][y] != DEAD_CELL)
  {
    block.map[x][y] = DEAD_CELL;
  }
  else if(n_neighbours == 3 && block.map[x][y] == DEAD_CELL)
  {
    revive_cell(block, x, y);
  }
  else if (n_neighbours > 3 && block.map[x][y] != DEAD_CELL)
  {
    block.map[x][y] = DEAD_CELL;
  }
  else
  {
    return;
  }
}

int equals(Block &current, Block &other)
{
  for (int i = 0; i < BLOCK_SIZE; i++)
  {
    for (int j = 0; j < BLOCK_SIZE; j++)
    {
      if(current.map[i][j] != other.map[i][j])
      {
        return 0;
      }
    }
  }
  return 1;
}
