#include <cstdio>
#include "crank_test.hpp"

void correct_valid_coordonates(Block *b, int x, int y, int expected)
{
  int output = valid_coordonate(x, y);
  if(output != expected)
  {
    fprintf(stderr, "FAILED TEST: valid coordonate\nOutput:\"%i\"\t \
            Expected:\"%i\"\n", output, expected);
  }
}

void correct_count_cell_neighbours(Block *b, int x, int y, int expected)
{
  int output = count_cell_neighbours(b, x, y);
  if(output != expected)
  {
    fprintf(stderr, "FAILED TEST: count cell neighbours\nOutput:\"%i\"\t \
            Expected:\"%i\"\n", output, expected);
  }
}

void correct_revive_cell(Block *b, int x, int y, CELL_TYPE expected)
{
  CELL_TYPE output = revive_cell(b, x, y);
  if(output != expected)
  {
    fprintf(stderr, "FAILED TEST: revive cell\nOutput:\"%u\"\t \
            Expected:\"%u\"\n", output, expected);
  }
}

void correct_crank_cell(Block *b, int x, int y, CELL_TYPE expected)
{
  CELL_TYPE output = crank_cell(b, x, y);
  if(output != expected)
  {
    fprintf(stderr, "FAILED TEST: crank cell\nOutput:\"%u\"\t \
            Expected:\"%u\"\n", output, expected);
  }
}

void correct_crank(Block *b, Block *expected)
{
  Block *result = crank(b);
  if(!equals(result, expected))
  {
    fprintf(stderr, "FAILED TEST: crank\n");
  }
  result->~Block();
}

void run_tests(void)
{
  printf("\nTEST:\n");
  Block *test1 = new Block(0, 0);
  //TEST: valid coordonates
  correct_valid_coordonates(test1, 0, 0, 1);
  correct_valid_coordonates(test1, 999, 999, 1);
  correct_valid_coordonates(test1, 0, 999, 1);
  correct_valid_coordonates(test1, 999, 0, 1);
  correct_valid_coordonates(test1, 0, 14, 1);
  correct_valid_coordonates(test1, 12, 0, 1);
  correct_valid_coordonates(test1, 20, 999, 1);
  correct_valid_coordonates(test1, 999, 30, 1);
  //TEST: count cell neighbours
  test1->map[10][10] = LIVE_CELL;
  test1->map[10][11] = LIVE_CELL;
  correct_count_cell_neighbours(test1, 10, 10, 1);
  test1->map[15][15] = LIVE_CELL;
  correct_count_cell_neighbours(test1, 15, 15, 0);
  test1->map[0][0] = LIVE_CELL;
  test1->map[0][1] = LIVE_CELL;
  test1->map[1][0] = LIVE_CELL;
  test1->map[1][1] = LIVE_CELL;
  correct_count_cell_neighbours(test1, 0, 0, 3);
  test1->map[1][0] = DEAD_CELL;
  correct_count_cell_neighbours(test1, 0, 0, 2);
  test1->map[50][50] = 2;
  test1->map[50][51] = 67;
  test1->map[51][50] = 47;
  test1->map[51][51] = 34;
  test1->map[49][50] = 13;
  test1->map[50][49] = 14;
  test1->map[49][51] = 7;
  test1->map[51][49] = 99;
  test1->map[49][49] = 33;
  correct_count_cell_neighbours(test1, 50, 50, 8);
  //TEST: revive cell
  test1->map[30][31] = LIVE_CELL;
  test1->map[31][30] = LIVE_CELL;
  test1->map[30][29] = LIVE_CELL;
  correct_revive_cell(test1, 30, 30, LIVE_CELL);
  test1->map[70][71] = 2;
  test1->map[71][70] = 4;
  test1->map[70][69] = 3;
  correct_revive_cell(test1, 70, 70, DEAD_CELL);
  test1->map[90][91] = 77;
  test1->map[91][90] = 77;
  test1->map[90][89] = 3;
  correct_revive_cell(test1, 90, 90, 77);
  test1->map[60][61] = 22;
  test1->map[61][60] = 22;
  test1->map[61][61] = 22;
  test1->map[60][59] = 3;
  correct_revive_cell(test1, 60, 60, DEAD_CELL);
  //TEST: crank cell
  test1->map[100][101] = LIVE_CELL;
  correct_crank_cell(test1, 100, 101, DEAD_CELL);
  test1->map[110][111] = 5;
  correct_crank_cell(test1, 110, 111, DEAD_CELL);
  test1->map[120][121] = 22;
  test1->map[120][119] = 22;
  test1->map[119][120] = 2;
  correct_crank_cell(test1, 120, 120, 22);
  test1->map[140][141] = 22;
  test1->map[140][139] = LIVE_CELL;
  test1->map[141][140] = 4;
  test1->map[139][140] = 2;
  correct_crank_cell(test1, 140, 140, DEAD_CELL);
  test1->~Block();
  Block *test2 = new Block(0, 0);
  Block *expected = new Block(0, 0);
  //TEST: crank
  test2->map[0][0] = LIVE_CELL;
  test2->map[3][0] = 3;
  test2->map[1][1] = 7;
  test2->map[2][1] = 4;
  test2->map[3][1] = 5;
  test2->map[0][2] = LIVE_CELL;
  test2->map[1][2] = LIVE_CELL;
  test2->map[2][2] = 22;
  test2->map[0][3] = LIVE_CELL;
  test2->map[1][3] = LIVE_CELL;
  test2->map[3][3] = LIVE_CELL;
  expected->map[0][3] = LIVE_CELL;
  expected->map[3][0] = 3;
  expected->map[3][1] = 5;
  correct_crank(test2, expected);
  expected->~Block();
}
