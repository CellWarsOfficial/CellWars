#include <crank.hpp>
#include <constants.hpp>
#include <block.hpp>
#include <cstdio>

/**
 * NB: The object taken as parameter by this function is destroyed
 * and the return object is a different one.
 **/
void crank(Block *block)
{
  Block *scratch = new Block(block);
  for(int i = 0; i < BLOCK_FULL; i++)
  {
    for(int j = 0; j < BLOCK_FULL; j++)
    {
      block->map[i][j] = crank_cell(scratch, i, j);
    }
  }
  delete scratch;
}

int valid_coordonate(int x, int y)
{
  return (0 <= x && x < BLOCK_FULL) && (0 <= y && y < BLOCK_FULL);
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
        for (int m = 0; m < h; m++)
        {
          if(block->map[x + i][y + j] == types[m])
          {
            result = types[m];
          }
        }
        types[h++] = block->map[x + i][y + j];
      }
    }
  }
  return result;
}

CELL_TYPE crank_cell(Block *block, int x, int y)
{
  int n_neighbours = count_cell_neighbours(block, x, y);
  CELL_TYPE result = block->map[x][y];
  if(n_neighbours < 2 && block->map[x][y] != DEAD_CELL) 
  {
    result = DEAD_CELL;
  } 
  else if (n_neighbours == 3 && block->map[x][y] == DEAD_CELL) 
  {
    result = revive_cell(block, x, y);
  } 
  else if (n_neighbours > 3 && block->map[x][y] != DEAD_CELL) 
  {
    result = DEAD_CELL;
  }
  return result;
}

int equals(Block *current, Block *other)
{
  for (int i = 0; i < BLOCK_FULL; i++)
  {
    for (int j = 0; j < BLOCK_FULL; j++)
    {
      if(current->map[i][j] != other->map[i][j])
      {
        return 0;
      }
    }
  }
  return 1;
}

//int dead_area(Block *block, int x, int y)
//{
//  return !(*(block->bitmap)[x * BITMAP_SIZE + y]);
//}


