#include <crank.hpp>
#include <constants.hpp>
#include <block.hpp>
#include <cstdio>

void Crank::crank(Block *block)
{
  crank_for(block, 1);
}

void Crank::crank_for(Block *block, int generations)
{
  // if k is not less than BLOCK_PADDING, sync problems may appear.
  // For single block, any k is fine.
  int i, j, k;
  for(k = 1; k <= generations; k++)
  {
    Block *scratch = new Block(block);
    for(i = k; i < BLOCK_FULL - k; i++)
    {
      for(j = k; j < BLOCK_FULL - k; j++)
      {
        block->set(i, j, crank_cell(scratch, i, j));
      }
    }
    delete scratch;
  }
}

int Crank::count_cell_neighbours(Block *block, int x, int y)
{
  int n = 0, i, j;
  for(i = -1; i < 2; i++)
  {
    for(j = -1; j < 2; j++)
    {
      if(block->map[x + i][y + j] != DEAD_CELL && !(i == 0 && j == 0))
      {
        n = n + 1;
      }
    }
  }
  return n;
}

CELL_TYPE Crank::revive_cell(Block *block, int x, int y)
{
  CELL_TYPE result = DEAD_CELL;
  CELL_TYPE types[3] = {DEAD_CELL, DEAD_CELL, DEAD_CELL};
  int h = 0, i, j;
  for(i = -1; i < 2; i++)
  {
    for(j = -1; j < 2; j++)
    {
      if(block->map[x + i][y + j] != DEAD_CELL && !(i == 0 && j == 0))
      {
        for (int k = 0; k < h; k++)
        {
          if(block->map[x + i][y + j] == types[k])
          {
            result = types[k];
          }
        }
        types[h++] = block->map[x + i][y + j];
      }
    }
  }
  return result;
}

CELL_TYPE Crank::crank_cell(Block *block, int x, int y)
{
  int n_neighbours = count_cell_neighbours(block, x, y);
  if(n_neighbours < 2) 
  {
    return DEAD_CELL;
  } 
  if (n_neighbours == 3 && block->map[x][y] == DEAD_CELL) 
  {
    return revive_cell(block, x, y);
  } 
  if (n_neighbours > 3) 
  {
    return DEAD_CELL;
  }
  return block->map[x][y];
}
