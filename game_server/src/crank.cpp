#include "crank.hpp"
#include "constants.hpp"
#include "block.hpp"
#include <cstdio>

/**
 * NB: The object taken as parameter by this function is destroyed
 * and the return object is a different one.
 **/
Block *crank(Block *block)
{
  Block *result = new Block(0, 0);
  for(int i = 0; i < BLOCK_SIZE; i++)
  {
    for(int j = 0; j < BLOCK_SIZE; j++)
    {
      result->map[i][j] = crank_cell(block, i, j);
    }
  }
  block->~Block();
  return result;
}

int valid_coordonate(int x, int y)
{
  return (0 <= x && x < BLOCK_SIZE) && (0 <= y && y < BLOCK_SIZE);
}

int count_cell_neighbours(Block *block, int x, int y)
{
  int n = 0;
  for(int i = -1; i < 2; i++)
  {
    for(int j = -1; j < 2; j++)
    {
      if(valid_coordonate(x + i, y + j) && !(i == 0 && j == 0) && 
              block->map[x + i][y + j] != DEAD_CELL)
      {
        n++;
      }
    }
  }
  return n;
}

CELL_TYPE revive_cell(Block *block, int x, int y)
{
  CELL_TYPE result = DEAD_CELL;
  CELL_TYPE types[3];
  int h = 0;
  for(int i = -1; i < 2; i++)
  {
    for(int j = -1; j < 2; j++)
    {
      if(count_cell_neighbours(block, x, y) == 3 && 
              valid_coordonate(x + i, y + j) && !(i == 0 && j == 0) && 
              block->map[x + i][y + j] != DEAD_CELL)
      {
        for (int m = 0; m < 3; m++)
        {
          if(block->map[x + i][y + j] == types[m])
          {
            block->map[x][y] = types[m];
            result = types[m];
          }
        }
        types[h] = block->map[x + i][y + j];
      }
    }
  }
  return result;
}

CELL_TYPE crank_cell(Block *block, int x, int y)
{
  int n_neighbours = count_cell_neighbours(block, x, y);
  CELL_TYPE result = DEAD_CELL;
  if(n_neighbours < 2 && block->map[x][y] != DEAD_CELL)
  {
    result =  DEAD_CELL;
  }
  else if(n_neighbours == 3 && block->map[x][y] == DEAD_CELL)
  {
    result = revive_cell(block, x, y);
  }
  else if (n_neighbours > 3 && block->map[x][y] != DEAD_CELL)
  {
    result = DEAD_CELL;
  }
  else
  {
    result = block->map[x][y]; 
  }
  return result;
}

int equals(Block *current, Block *other)
{
  for (int i = 0; i < BLOCK_SIZE; i++)
  {
    for (int j = 0; j < BLOCK_SIZE; j++)
    {
      if(current->map[i][j] != other->map[i][j])
      {
        return 0;
      }
    }
  }
  return 1;
}

void set_area(Block *block, int x, int y)
{



}

int prune(Block *block, int x, int y)
{
  





}
