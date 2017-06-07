#include <crank.hpp>
#include <constants.hpp>
#include <block.hpp>
#include <cstdio>

/**
 * NB: The object taken as parameter by this function is destroyed
 * and the return object is a different one.
 **/

void Crank::crank(Block *block)
{
  Block *scratch = new Block(block);
  for(int i = 0; i < BLOCK_FULL; i++)
  {
    for(int j = 0; j < BLOCK_FULL; j++)
    {
      int jump = prune_area(scratch, i, j);
      if(jump == 0)
      {
        CELL_TYPE inp = scratch->map[i][j];
        CELL_TYPE out = crank_cell(scratch, i, j);
        int new_count = get_area_count(i, j, inp, out); 
        update_area_count(block, i, j, new_count);
        block->set(i, j, out);
      }
      else
      {
        i = i + jump - 1;
      }
    }
  }
  delete scratch;
}

int Crank::valid_coordonate(int x, int y)
{
  return 0 <= x && x < BLOCK_FULL && 0 <= y && y < BLOCK_FULL;
}

int Crank::count_cell_neighbours(Block *block, int x, int y)
{
  int n = 0;
  for(int i = -1; i < 2; i++)
  {
    for(int j = -1; j < 2; j++)
    {
      if(valid_coordonate(x + i, y + j) && !(i == 0 && j == 0) && 
              block->map[x + i][y + j] != DEAD_CELL)
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

CELL_TYPE Crank::crank_cell(Block *block, int x, int y)
{
  int n_neighbours = count_cell_neighbours(block, x, y);
  if(n_neighbours < 2 && block->map[x][y] != DEAD_CELL) 
  {
    return  DEAD_CELL;
  } 
  else if (n_neighbours == 3 && block->map[x][y] == DEAD_CELL) 
  {
    return revive_cell(block, x, y);
  } 
  else if (n_neighbours > 3 && block->map[x][y] != DEAD_CELL) 
  {
    return DEAD_CELL;
  }
  return block->map[x][y];
}

int Crank::equals(Block *current, Block *other)
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

int Crank::prune_area(Block *block, int x, int y)
{
  int p_x = x / 10;
  int p_y = y / 10;
  if(block->counts[p_x + BLOCK_FULL * p_y] == 0)
  {
    return PRUNE_SIZE - x % PRUNE_SIZE;
  }
  return 0;
}

int Crank::get_area_count(int x, int y, CELL_TYPE before, CELL_TYPE after)
{
  if(before == DEAD_CELL && after != DEAD_CELL)
  {
    return 1;
  }
  else if(before != DEAD_CELL && after == DEAD_CELL)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

void Crank::inc_area_count(Block *block, int x, int y)
{
  int p_x = x / 10;
  int p_y = y / 10;
  block->counts[p_x + BLOCK_FULL * p_y]++;
}

void Crank::dec_area_count(Block *block, int x, int y)
{
  int p_x = x / 10;
  int p_y = y / 10;
  block->counts[p_x + BLOCK_FULL * p_y]--;
}

void Crank::update_area_count(Block *block, int x, int y, int c)
{
  int p_x = x / 10;
  int p_y = y / 10;
  block->counts[p_x + BLOCK_FULL * p_y] += c;
}
