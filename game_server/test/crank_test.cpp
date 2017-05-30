#include <cstdio>
#include "crank_test.hpp"
#include "../src/all.hpp"

void correct_valid_coordonate(Block *b, int x, int y, int expected)
{
  int output = valid_coordonate(x, y);
  if(output != expected)
  {
    fprintf(stderr, "FAILED TEST: valid coordonate\nOutput:\"%i\"\tExpected:\"%i\"\n", output, expected);
  }
}

void count_cell_neighbours(Block *b, int x, int y, int expected)
{
  int output = count_cell_neighbours(b, x, y);
  if(output != expected)
  {
    fprintf(stderr, "FAILED TEST: count cell neighbours\nOutput:\"%i\"\tExpected:\"%i\"\n", output, expected);
  }
}

void correct_revive_cell(Block *b, int x, int y, CELL_TYPE expected)
{
  revive_cell(b, x, y);
  CELL_TYPE output = b->map[x][y];
  if(output != expected)
  {
    fprintf(stderr, "FAILED TEST: revive cell\nOutput:\"%u\"\tExpected:\"%u\"\n", output, expected);
  }
}

void correct_crank_cell(Block *b, int x, int y, CELL_TYPE expected)
{
  crank_cell(b, x, y);
  CELL_TYPE output = b->map[x][y]; 
  if(output != expected)
  {
    fprintf(stderr, "FAILED TEST: revive cell\nOutput:\"%u\"\tExpected:\"%u\"\n", output, expected);
  }
}

void correct_crank(Block *b, Block *expected)
{
  crank(b);
  if(equals(b, expected))
  {
    fprintf(stderr, "FAILED TEST: crank\n");
  }
}

int main(void)
{
  Block *test1 = new Block(0, 0);
  //test1.map[10][10] = 1;
  //test1.map[10][10] = LIVE_CELL;
  //test1.map[10][11] = LIVE_CELL;
  //test1.map[15][15] = LIVE_CELL;
  correct_valid_coordonates(test1, 10, 14, 1);
  //void correct_count_cell_neighbours(Block *b, int x, int y, int expected);
  //void correct_revive_cell(Block *b, int x, int y, CELL_TYPE expected);
  //void correct_crank_cell(Block *b, int x, int y, CELL_TYPE expected);
  //void correct_valid_coordonates(Block *b, Block *expected);

  return 0;
}
