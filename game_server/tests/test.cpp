#include <all.hpp>

void Tests::correct_valid_coordonate(Block *b, int x, int y, int expected)
{
  if(expected != b.correct_valid_coordonate(x, y))
  {
    printf("%s", "FAILED TEST: valid coordonate\n");
  }
}

void Tests::count_cell_neighbours(Block *b, int x, int y, int expected)
{
  if(expected != b.count_cell_neighbours(x, y))
  {
    printf("%s", "FAILED TEST: count cell neighbours\n");
  } 
}

void Tests::revive_cell(Block *b, int x, int y, CELL_TYPE expected_type)
{
  b.revive(x, y);
  if(expected_type != b.map[x][y])
  {
    printf("%s", "FAILED TEST: revive cell\n");
  } 
}

void Tests::correct_crank_cell(Block *b, int x, int y, CELL_TYPE expected_type)
{
  b.crank_cell(x, y);
  if(b.map[x][y] != expected_type)
  {
    printf("%s", "FAILED TEST: crank cell\n");
  }
}

void Tests::correct_crank(Block *b, Block *expected_b)
{
  b.crank();
  if(!b.equals(expected_b))
  {
    printf("%s", "FAILED TEST: crank\n");
  }
}
