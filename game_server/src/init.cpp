#include "all.hpp"
#include <cstdio>
#include <cstring>
#include <string>

using namespace std;

int equ(char *s1, string s2)
{
  int i = 0;
  for(; (s1[i] != 0) && (s2[i] != 0) && (s1[i] == s2[i]); i++);
  return s1[i] == s2[i];
}

bool use_safety = 1;

int main(int argc, char **argv)
{

#if defined(CRANK_TEST_HPP)

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
  correct_crank(test2, expected);
  delete expected;
  return 0;

#elif !defined(CRANK_TEST_HPP)

  printf("\nINIT:\n");
  int i;
  for(i = 1; i < argc; i++)
  {
    if(equ(argv[i], "-no_safe"))
    {
      use_safety = 0;
      continue;
    }
    fprintf(stderr, "Unrecognised argument \"%s\"\nIgnoring.\n", argv[i]);
  }

#endif

}

