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
  if(generations <= 0)
  {
    return;
  }
  // if k is not less than BLOCK_PADDING, sync problems may appear.
  // For single block, any k is fine.
  int i, j, k;
  Block *scratch, *aux, *pseudo_block;
/* Hot potato with scratch
input - B
output - B
pseudo_block disguised as scratch - s
pseudo_block - b
"->" - crank
"=" - copy from left to right 
1:    B
      b ->  B // no scratch required
2:    B
      s -> b -> B
3:    B
      b -> s -> b -> B
2x:   B
      s -> b -> s -> b -> ... -> b -> B
2x+1: B
      b -> s -> b -> s -> ... -> b -> B

bssb makes nice dresses >.>
 */
  pseudo_block = new Block(block);
  if(generations != 1)
  {
    scratch = new Block(block);
    for(k = 1; k < generations; k++)
    {
      aux = scratch;
      scratch = pseudo_block;
      pseudo_block = aux;
      for(i = k; i < BLOCK_FULL - k; i++)
      {
        for(j = k; j < BLOCK_FULL - k; j++)
        {
          pseudo_block->set(i, j, crank_cell(scratch, i, j));
        }
      }
    }
    delete scratch;
  }
  for(i = generations; i < BLOCK_FULL - generations; i++)
  {
    for(j = generations; j < BLOCK_FULL - generations; j++)
    {
      block->set(i, j, crank_cell(pseudo_block, i, j));
    }
  }
  delete pseudo_block;
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
