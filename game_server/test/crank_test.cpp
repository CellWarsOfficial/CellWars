#include <cstdio>
#include "crank_test.hpp"

int tests = 0;
int fails = 0;

int equals(Block *current, Block *other, int x1, int y1, int x2, int y2)
{
  int result = 1, i, j;
  for (i = x1; i <= x2; i++)
  {
    for (j = y1; j <= y2; j++)
    {
      if(current->map[i][j] != other->map[i][j])
      {
        fprintf(stderr, "%i\t%i\tEXPECTED: %i\tRESULT: %i", i, j
               , other->map[i][j], current->map[i][j]);
        result = 0;
      }
    }
  }
  return result;
}

void correct_valid_coordonates(Block *b, Action *a, int x, int y, int expected)
{
  tests++;
  int output = valid_coordonate(x, y);
  if(output != expected)
  {
    fails++;
    fprintf(stderr, "FAILED TEST: valid coordonate\nOutput:\"%i\"\t \
            Expected:\"%i\"\n", output, expected);
  }
}

void correct_count_cell_neighbours(Block *b, Action *a, int x, int y, int expected)
{
  tests++;
  int output = a->count_cell_neighbours(b, x, y);
  if(output != expected)
  {
    fails++;
    fprintf(stderr, "FAILED TEST: count cell neighbours\nOutput:\"%i\"\t \
            Expected:\"%i\"\n", output, expected);
  }
}

void correct_revive_cell(Block *b, Action *a, int x, int y, CELL_TYPE expected)
{
  tests++;
  CELL_TYPE output = a->revive_cell(b, x, y);
  if(output != expected)
  {
    fails++;
    fprintf(stderr, "FAILED TEST: revive cell\nOutput:\"%u\"\t \
            Expected:\"%u\"\n", output, expected);
  }
}

void correct_crank_cell(Block *b, Action *a, int x, int y, CELL_TYPE expected)
{
  tests++;
  CELL_TYPE output = a->crank_cell(b, x, y);
  if(output != expected)
  {
    fails++;
    fprintf(stderr, "FAILED TEST: crank cell\nOutput:\"%u\"\t \
            Expected:\"%u\"\n", output, expected);
  }
}

void correct_crank_for(Block *b, Block *expected, Action *a, int gens)
{
  tests++;
  a->crank_for(b, gens);
  if(!equals(b, expected, gens, gens, BLOCK_FULL - gens - 1, BLOCK_FULL - gens - 1))
  {
    fails++;
    fprintf(stderr, "FAILED TEST: crank\n");
  }
}

void correct_crank(Block *b, Block *expected, Action *a)
{
  correct_crank_for(b, expected, a, 1);
}

int main(void)
{
  Block *test1 = new Block(0, 0);
  Action *action = new Crank();
  //TEST: valid coordonates
  correct_valid_coordonates(test1, action, 0, 0, 1);
  correct_valid_coordonates(test1, action, BLOCK_FULL - 1, BLOCK_FULL - 1, 1);
  correct_valid_coordonates(test1, action, 0, BLOCK_FULL - 1, 1);
  correct_valid_coordonates(test1, action, BLOCK_FULL - 1, 0, 1);
  correct_valid_coordonates(test1, action, 0, 14, 1);
  correct_valid_coordonates(test1, action, 12, 0, 1);
  correct_valid_coordonates(test1, action, 20, BLOCK_FULL - 1, 1);
  correct_valid_coordonates(test1, action, BLOCK_FULL - 1, 30, 1);
  correct_valid_coordonates(test1, action, -1, 0, 0);
  correct_valid_coordonates(test1, action, 0, -5, 0);
  correct_valid_coordonates(test1, action, -2, -3, 0);
  correct_valid_coordonates(test1, action, 2 * BLOCK_FULL, 0, 0);
  correct_valid_coordonates(test1, action, 0, BLOCK_FULL, 0);
  correct_valid_coordonates(test1, action, -(BLOCK_FULL - 1), BLOCK_FULL - 1, 0);
  //TEST: count cell neighbours
  test1->map[10][10] = LIVE_CELL;
  test1->map[10][11] = LIVE_CELL;
  correct_count_cell_neighbours(test1, action, 10, 10, 1);
  test1->map[15][15] = LIVE_CELL;
  correct_count_cell_neighbours(test1, action, 15, 15, 0);
  test1->map[1][1] = LIVE_CELL;
  test1->map[1][2] = LIVE_CELL;
  test1->map[2][1] = LIVE_CELL;
  test1->map[2][2] = LIVE_CELL;
  correct_count_cell_neighbours(test1, action, 1, 1, 3);
  test1->map[2][1] = DEAD_CELL;
  correct_count_cell_neighbours(test1, action, 1, 1, 2);
  test1->map[50][50] = 2;
  test1->map[50][51] = 67;
  test1->map[51][50] = 47;
  test1->map[51][51] = 34;
  test1->map[49][50] = 13;
  test1->map[50][49] = 14;
  test1->map[49][51] = 7;
  test1->map[51][49] = 99;
  test1->map[49][49] = 33;
  correct_count_cell_neighbours(test1, action, 50, 50, 8);
  //TEST: revive cell
  test1->map[30][31] = LIVE_CELL;
  test1->map[31][30] = LIVE_CELL;
  test1->map[30][29] = LIVE_CELL;
  correct_revive_cell(test1, action, 30, 30, LIVE_CELL);
  test1->map[70][71] = 2;
  test1->map[71][70] = 4;
  test1->map[70][69] = 3;
  correct_revive_cell(test1, action, 70, 70, DEAD_CELL);
  test1->map[90][91] = 77;
  test1->map[91][90] = 77;
  test1->map[90][89] = 3;
  correct_revive_cell(test1, action, 90, 90, 77);
  //TEST: crank cell
  test1->map[100][101] = LIVE_CELL;
  correct_crank_cell(test1, action, 100, 101, DEAD_CELL);
  test1->map[110][111] = 5;
  correct_crank_cell(test1, action, 110, 111, DEAD_CELL);
  test1->map[120][121] = 22;
  test1->map[120][119] = 22;
  test1->map[119][120] = 2;
  correct_crank_cell(test1, action, 120, 120, 22);
  test1->map[140][141] = 22;
  test1->map[140][139] = LIVE_CELL;
  test1->map[141][140] = 4;
  test1->map[139][140] = 2;
  correct_crank_cell(test1, action, 140, 140, DEAD_CELL);
  delete test1;
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
  correct_crank(test2, expected, action);
  
  //TEST for :
  //.............................
  //.............................
  //...........PPGGG.............
  //..........P..P...............
  //...........PP................
  //...........GG................
  //...........GG................
  //.............................
  //.............................
  //where P has type 1 and G has
  //type 2.
  //The coordonate od first P
  //is (10, 10).
  Block *test3 = new Block(0, 0);
  test3->map[12][10] = 2;
  test3->map[13][10] = 2;
  test3->map[14][10] = 2;
  test3->map[10][10] = 1;
  test3->map[11][10] = 1;
  test3->map[9][9] = 1;
  test3->map[12][9] = 1;
  test3->map[10][8] = 1;
  test3->map[11][8] = 1;
  test3->map[10][7] = 2;
  test3->map[11][7] = 2;
  test3->map[10][6] = 2;
  test3->map[11][6] = 2;
  correct_crank_cell(test3, action, 12, 10, 2);
  correct_crank_cell(test3, action, 13, 10, 2);
  correct_crank_cell(test3, action, 14, 7, DEAD_CELL);
  
  correct_crank_cell(test3, action, 10, 10, 1);
  correct_crank_cell(test3, action, 11, 10, 1);

  correct_crank_cell(test3, action, 9, 9, 1);
  correct_crank_cell(test3, action, 12, 9, DEAD_CELL);


  correct_crank_cell(test3, action, 10, 8, DEAD_CELL);
  correct_crank_cell(test3, action, 11, 8, DEAD_CELL);

  correct_crank_cell(test3, action, 10, 7, DEAD_CELL);
  correct_crank_cell(test3, action, 11, 7, DEAD_CELL);
  correct_crank_cell(test3, action, 10, 6, 2);
  correct_crank_cell(test3, action, 11, 6, 2);
  delete test3;
  Block *test4 = new Block(0, 0);
  test4->map[12][10] = 2;
  test4->map[13][10] = 2;
  test4->map[14][10] = 2;
  test4->map[10][10] = 1;
  test4->map[11][10] = 1;
  test4->map[9][9] = 1;
  test4->map[12][9] = 1;
  test4->map[10][8] = 1;
  test4->map[11][8] = 1;
  test4->map[10][7] = 2;
  test4->map[11][7] = 2;
  test4->map[10][6] = 2;
  test4->map[11][6] = 2;

  Block *expected2 = new Block(0, 0);

  expected2->map[12][10] = 2;
  expected2->map[13][10] = 2;
  expected2->map[14][10] = DEAD_CELL;

  expected2->map[10][10] = 1;
  expected2->map[11][10] = 1;

  expected2->map[9][9] = 1;
  expected2->map[12][9] = DEAD_CELL;


  expected2->map[10][8] = DEAD_CELL;
  expected2->map[11][8] = DEAD_CELL;

  expected2->map[10][7] = DEAD_CELL;
  expected2->map[11][7] = DEAD_CELL;
  expected2->map[10][6] = 2;
  expected2->map[11][6] = 2;

  expected2->map[9][7] = 2;
  expected2->map[9][8] = 1;
  expected2->map[11][11] = 1;
  expected2->map[12][7] = 2;
  expected2->map[12][8] = 1;
  expected2->map[12][11] = 2;
  expected2->map[13][11] = 2;

  correct_count_cell_neighbours(test4, action, 9, 7, 3);

  correct_crank(test4, expected2, action);
  delete test4;
  delete expected2;
  delete test2;
  delete expected;

  Block *test5 = new Block(0, 0);
  test5 -> map[21][27] = 1;
  test5 -> map[21][28] = 1;
  test5 -> map[22][27] = 1;
  test5 -> map[22][28] = 1;

  test5 -> map[25][21] = 1;
  test5 -> map[25][22] = 1;
  test5 -> map[26][21] = 1;
  test5 -> map[26][22] = 1;

  test5 -> map[31][25] = 1;
  test5 -> map[31][26] = 1;
  test5 -> map[32][25] = 1;
  test5 -> map[32][26] = 1;

  test5 -> map[27][31] = 1;
  test5 -> map[27][32] = 1;
  test5 -> map[28][31] = 1;
  test5 -> map[28][32] = 1;

  test5 -> map[24][25] = 2;
  test5 -> map[24][26] = 2;
  test5 -> map[24][27] = 2;
  test5 -> map[24][28] = 2;

  test5 -> map[25][24] = 2;
  test5 -> map[26][24] = 2;
  test5 -> map[27][24] = 2;
  test5 -> map[28][24] = 2;

  test5 -> map[25][29] = 2;
  test5 -> map[26][29] = 2;
  test5 -> map[27][29] = 2;
  test5 -> map[28][29] = 2;

  test5 -> map[29][25] = 2;
  test5 -> map[29][26] = 2;
  test5 -> map[29][27] = 2;
  test5 -> map[29][28] = 2;

  test5 -> map[25][27] = 2;
  test5 -> map[26][26] = 3;
  test5 -> map[27][26] = 3;

  Block *test6 = new Block(test5);

  correct_crank_for(test5, test6, action, 0);
  correct_crank_for(test5, test6, action, 4);
  correct_crank_for(test5, test6, action, 8);

  Block *test7 = new Block(test6);

  test7 -> map[25][27] = 0;
  test7 -> map[26][26] = 0;
  test7 -> map[27][26] = 0;
  test7 -> map[26][27] = 3;
  test7 -> map[27][27] = 3;
  test7 -> map[28][26] = 2;

  correct_crank_for(test5, test7, action, 2);
  correct_crank_for(test5, test6, action, 2);
  correct_crank_for(test5, test7, action, 6);
  delete test5;
  delete test6;
  delete test7;
  delete action;

  fprintf(stderr, "%d/%d tests passed\n", tests - fails, tests);
  return fails;
}
