#include "block_test.hpp"
#include <cstring>
#include <cstdio>
#include <set>

using namespace std;

int fails = 0;
int tests = 0;
Block *b1;
Block *b2;

void reset_region_flags(Block *b)
{
  memset(b -> border_changes
    , 0
    , BLOCK_NEIGHBOURING_REGIONS * sizeof(uint8_t)
    );
}

void validate_block(Block *b)
{
  int i, j;
  tests++;
  int fail = 0;
  for(i = 0; i < BLOCK_FULL; i++)
  {
    for(j = 0; j < BLOCK_FULL; j++)
    {
      if((b -> map[i][j] == 1) || (b -> map[i][j] == 2))
      {
        fail = 1;
        fprintf(stderr, "%d: imported outside region %d %d.\n", tests, i, j);
      }
    }
  }
  fails += fail;
}

void set_inside(Block *b)
{
  int i, j;
  for(i = BLOCK_PADDING; i < BLOCK_SIZE + BLOCK_PADDING; i++)
  {
    for(j = BLOCK_PADDING; j < BLOCK_SIZE + BLOCK_PADDING; j++)
    {
      b -> map[i][j] = 3;
    }
  }
}

void empty_inside(Block *b) // aren't we all?
{
  int i, j;
  tests++;
  int fail = 0;
  for(i = BLOCK_PADDING; i < BLOCK_SIZE + BLOCK_PADDING; i++)
  {
    for(j = BLOCK_PADDING; j < BLOCK_SIZE + BLOCK_PADDING; j++)
    {
      if(b -> map[i][j] != 3)
      {
        fail = 1;
        fprintf(stderr, "%d: overwritten interior %d %d.\n", tests, i, j);
      }
    }
  }
  fails += fail;
}

void expect_clean_flags(Block *b)
{
  tests++;
  int fail = 0;
  for(int i = 0; i < BLOCK_NEIGHBOURING_REGIONS; i++)
  {
    if(b -> border_changes[i] != 0)
    {
      fail = 1;
      fprintf(stderr, "%d: flag changed when not expected to in region %d.\n", tests, i);
    }
  }
  fails += fail;
}

void expect(uint8_t value1, uint8_t value2)
{
  tests++;
  if(value1 != value2)
  {
    fails++;
    fprintf(stderr, "%d: unmatching flags, got %d expected %d.\n", tests, value1, value2);
  }
}

void expect_cell(CELL_TYPE value1, CELL_TYPE value2)
{
  tests++;
  if(value1 != value2)
  {
    fails++;
    fprintf(stderr, "%d: unmatching cells, got %d expected %d.\n", tests, value1, value2);
  }
}

void expect_int(int value1, int value2)
{
  tests++;
  if(value1 != value2)
  {
    fails++;
    fprintf(stderr, "%d: unmatching integers, got %d expected %d.\n", tests, value1, value2);
  }
}

void check_region_change_flag()
{
  // NW
  b1 -> set(0, 0, 2);
  b1 -> set(BLOCK_PADDING - 1, BLOCK_PADDING - 1, 2);
  expect_clean_flags(b1); // 1
  reset_region_flags(b1);
  b1 -> set(BLOCK_PADDING, BLOCK_PADDING, 1);
  expect(b1 -> border_changes[P_REGION_NW], 1); // 2
  expect(b1 -> border_changes[P_REGION_N], 1); // 3
  expect(b1 -> border_changes[P_REGION_W], 1); // 4
  b1 -> border_changes[P_REGION_NW] = 0;
  b1 -> border_changes[P_REGION_N] = 0;
  b1 -> border_changes[P_REGION_W] = 0;
  expect_clean_flags(b1); // 5
  reset_region_flags(b1);
  b1 -> set(2 * BLOCK_PADDING - 1, 2 * BLOCK_PADDING - 1, 1);
  expect(b1 -> border_changes[P_REGION_NW], 1); // 6
  expect(b1 -> border_changes[P_REGION_N], 1); // 7
  expect(b1 -> border_changes[P_REGION_W], 1); // 8
  b1 -> border_changes[P_REGION_NW] = 0;
  b1 -> border_changes[P_REGION_N] = 0;
  b1 -> border_changes[P_REGION_W] = 0;
  expect_clean_flags(b1); // 9
  reset_region_flags(b1);
  b1 -> set(2 * BLOCK_PADDING, 2 * BLOCK_PADDING, 2);
  expect_clean_flags(b1); // 10
  reset_region_flags(b1);
  // SE
  b1 -> set(BLOCK_FULL - 1, BLOCK_FULL - 1, 2);
  b1 -> set(BLOCK_SIZE + BLOCK_PADDING, BLOCK_SIZE + BLOCK_PADDING, 2);
  expect_clean_flags(b1); // 11
  reset_region_flags(b1);
  b1 -> set(BLOCK_SIZE + BLOCK_PADDING - 1, BLOCK_SIZE + BLOCK_PADDING - 1, 1);
  expect(b1 -> border_changes[P_REGION_SE], 1); // 12
  expect(b1 -> border_changes[P_REGION_S], 1); // 13
  expect(b1 -> border_changes[P_REGION_E], 1); // 14
  b1 -> border_changes[P_REGION_SE] = 0;
  b1 -> border_changes[P_REGION_S] = 0;
  b1 -> border_changes[P_REGION_E] = 0;
  expect_clean_flags(b1); // 15
  reset_region_flags(b1);
  b1 -> set(BLOCK_SIZE, BLOCK_SIZE, 1);
  expect(b1 -> border_changes[P_REGION_SE], 1); // 16
  expect(b1 -> border_changes[P_REGION_S], 1); // 17
  expect(b1 -> border_changes[P_REGION_E], 1); // 18
  b1 -> border_changes[P_REGION_SE] = 0;
  b1 -> border_changes[P_REGION_S] = 0;
  b1 -> border_changes[P_REGION_E] = 0;
  expect_clean_flags(b1); // 19
  reset_region_flags(b1);
  b1 -> set(BLOCK_SIZE - 1, BLOCK_SIZE - 1, 2);
  expect_clean_flags(b1); // 20
  reset_region_flags(b1);
  // NE
  b1 -> set(BLOCK_PADDING - 1, BLOCK_FULL - 1, 2);
  b1 -> set(BLOCK_SIZE + BLOCK_PADDING, BLOCK_SIZE + BLOCK_PADDING, 2);
  expect_clean_flags(b1); // 21
  reset_region_flags(b1);
  b1 -> set(BLOCK_PADDING, BLOCK_SIZE + BLOCK_PADDING - 1, 1);
  expect(b1 -> border_changes[P_REGION_NE], 1); // 22
  expect(b1 -> border_changes[P_REGION_N], 1); // 23
  expect(b1 -> border_changes[P_REGION_E], 1); // 24
  b1 -> border_changes[P_REGION_NE] = 0;
  b1 -> border_changes[P_REGION_N] = 0;
  b1 -> border_changes[P_REGION_E] = 0;
  expect_clean_flags(b1); // 25
  reset_region_flags(b1);
  b1 -> set(2 * BLOCK_PADDING - 1, BLOCK_SIZE, 1);
  expect(b1 -> border_changes[P_REGION_NE], 1); // 26
  expect(b1 -> border_changes[P_REGION_N], 1); // 27
  expect(b1 -> border_changes[P_REGION_E], 1); // 28
  b1 -> border_changes[P_REGION_NE] = 0;
  b1 -> border_changes[P_REGION_N] = 0;
  b1 -> border_changes[P_REGION_E] = 0;
  expect_clean_flags(b1); // 29
  reset_region_flags(b1);
  b1 -> set(2 * BLOCK_PADDING, BLOCK_SIZE - 1, 2);
  expect_clean_flags(b1); // 30
  reset_region_flags(b1);
  // SW
  b1 -> set(BLOCK_FULL - 1, BLOCK_PADDING - 1, 2);
  b1 -> set(BLOCK_SIZE + BLOCK_PADDING, BLOCK_SIZE + BLOCK_PADDING, 2);
  expect_clean_flags(b1); // 31
  reset_region_flags(b1);
  b1 -> set(BLOCK_SIZE + BLOCK_PADDING - 1, BLOCK_PADDING, 1);
  expect(b1 -> border_changes[P_REGION_SW], 1); // 32
  expect(b1 -> border_changes[P_REGION_S], 1); // 33
  expect(b1 -> border_changes[P_REGION_W], 1); // 34
  b1 -> border_changes[P_REGION_SW] = 0;
  b1 -> border_changes[P_REGION_S] = 0;
  b1 -> border_changes[P_REGION_W] = 0;
  expect_clean_flags(b1); // 35
  reset_region_flags(b1);
  b1 -> set(BLOCK_SIZE, 2 * BLOCK_PADDING - 1, 1);
  expect(b1 -> border_changes[P_REGION_SW], 1); // 36
  expect(b1 -> border_changes[P_REGION_S], 1); // 37
  expect(b1 -> border_changes[P_REGION_W], 1); // 38
  b1 -> border_changes[P_REGION_SW] = 0;
  b1 -> border_changes[P_REGION_S] = 0;
  b1 -> border_changes[P_REGION_W] = 0;
  expect_clean_flags(b1); // 39
  reset_region_flags(b1);
  b1 -> set(BLOCK_SIZE - 1, 2 * BLOCK_PADDING, 2);
  expect_clean_flags(b1); // 40
  reset_region_flags(b1);
}

void check_sync()
{
  // NW
  b2 = new Block(0, 0);
  set_inside(b2);
  b2 -> sync_with(b1, P_REGION_NW);
  expect_cell(b2 -> map[BLOCK_FULL - 1][BLOCK_FULL - 1], 1); // 41
  expect_cell(b2 -> map[BLOCK_SIZE + BLOCK_PADDING][BLOCK_SIZE + BLOCK_PADDING], 1); // 42
  b2 -> map[BLOCK_FULL - 1][BLOCK_FULL - 1] = 0;
  b2 -> map[BLOCK_SIZE + BLOCK_PADDING][BLOCK_SIZE + BLOCK_PADDING] = 0;
  validate_block(b2); // 43
  empty_inside(b2); // 44
  delete b2;
  // SE
  b2 = new Block(0, 0);
  set_inside(b2);
  b2 -> sync_with(b1, P_REGION_SE);
  expect_cell(b2 -> map[0][0], 1); // 45
  expect_cell(b2 -> map[BLOCK_PADDING - 1][BLOCK_PADDING - 1], 1); // 46
  b2 -> map[0][0] = 0;
  b2 -> map[BLOCK_PADDING - 1][BLOCK_PADDING - 1] = 0;
  validate_block(b2); // 47
  empty_inside(b2); // 48
  delete b2;
  // NE
  b2 = new Block(0, 0);
  set_inside(b2);
  b2 -> sync_with(b1, P_REGION_NE);
  expect_cell(b2 -> map[BLOCK_FULL - 1][0], 1); // 49
  expect_cell(b2 -> map[BLOCK_SIZE + BLOCK_PADDING][BLOCK_PADDING - 1], 1); // 50
  b2 -> map[BLOCK_FULL - 1][0] = 0;
  b2 -> map[BLOCK_SIZE + BLOCK_PADDING][BLOCK_PADDING - 1] = 0;
  validate_block(b2); // 51
  empty_inside(b2); // 52
  delete b2;
  // SW
  b2 = new Block(0, 0);
  set_inside(b2);
  b2 -> sync_with(b1, P_REGION_SW);
  expect_cell(b2 -> map[0][BLOCK_FULL - 1], 1); // 53
  expect_cell(b2 -> map[BLOCK_PADDING - 1][BLOCK_SIZE + BLOCK_PADDING], 1); // 54
  b2 -> map[0][BLOCK_FULL - 1] = 0;
  b2 -> map[BLOCK_PADDING - 1][BLOCK_SIZE + BLOCK_PADDING] = 0;
  validate_block(b2); // 55
  empty_inside(b2); // 56
  delete b2;
  // N
  b2 = new Block(0, 0);
  set_inside(b2);
  b2 -> sync_with(b1, P_REGION_N);
  expect_cell(b2 -> map[BLOCK_SIZE + BLOCK_PADDING][BLOCK_PADDING], 1); // 57
  expect_cell(b2 -> map[BLOCK_FULL - 1][2 * BLOCK_PADDING - 1], 1); // 58
  expect_cell(b2 -> map[BLOCK_FULL - 1][BLOCK_SIZE], 1); // 59
  expect_cell(b2 -> map[BLOCK_SIZE + BLOCK_PADDING][BLOCK_SIZE + BLOCK_PADDING - 1], 1); // 60
  b2 -> map[BLOCK_SIZE + BLOCK_PADDING][BLOCK_PADDING] = 0;
  b2 -> map[BLOCK_FULL - 1][2 * BLOCK_PADDING - 1] = 0;
  b2 -> map[BLOCK_FULL - 1][BLOCK_SIZE] = 0;
  b2 -> map[BLOCK_SIZE + BLOCK_PADDING][BLOCK_SIZE + BLOCK_PADDING - 1] = 0;
  validate_block(b2); // 61
  empty_inside(b2); // 62
  delete b2;
  // S
  b2 = new Block(0, 0);
  set_inside(b2);
  b2 -> sync_with(b1, P_REGION_S);
  expect_cell(b2 -> map[BLOCK_PADDING - 1][BLOCK_PADDING], 1); // 63
  expect_cell(b2 -> map[0][2 * BLOCK_PADDING - 1], 1); // 64
  expect_cell(b2 -> map[0][BLOCK_SIZE], 1); // 65
  expect_cell(b2 -> map[BLOCK_PADDING - 1][BLOCK_SIZE + BLOCK_PADDING - 1], 1); // 66
  b2 -> map[BLOCK_PADDING - 1][BLOCK_PADDING] = 0;
  b2 -> map[0][2 * BLOCK_PADDING - 1] = 0;
  b2 -> map[0][BLOCK_SIZE] = 0;
  b2 -> map[BLOCK_PADDING - 1][BLOCK_SIZE + BLOCK_PADDING - 1] = 0;
  validate_block(b2); // 67
  empty_inside(b2); // 68
  delete b2;
  // W
  b2 = new Block(0, 0);
  set_inside(b2);
  b2 -> sync_with(b1, P_REGION_W);
  expect_cell(b2 -> map[BLOCK_PADDING][BLOCK_SIZE + BLOCK_PADDING], 1); // 69
  expect_cell(b2 -> map[2 * BLOCK_PADDING - 1][BLOCK_FULL - 1], 1); // 70
  expect_cell(b2 -> map[BLOCK_SIZE][BLOCK_FULL - 1], 1); // 71
  expect_cell(b2 -> map[BLOCK_SIZE + BLOCK_PADDING - 1][BLOCK_SIZE + BLOCK_PADDING], 1); // 72
  b2 -> map[BLOCK_PADDING][BLOCK_SIZE + BLOCK_PADDING] = 0;
  b2 -> map[2 * BLOCK_PADDING - 1][BLOCK_FULL - 1] = 0;
  b2 -> map[BLOCK_SIZE][BLOCK_FULL - 1] = 0;
  b2 -> map[BLOCK_SIZE + BLOCK_PADDING - 1][BLOCK_SIZE + BLOCK_PADDING] = 0;
  validate_block(b2); // 73
  empty_inside(b2); // 74
  delete b2;
  // E
  b2 = new Block(0, 0);
  set_inside(b2);
  b2 -> sync_with(b1, P_REGION_E);
  expect_cell(b2 -> map[BLOCK_PADDING][BLOCK_PADDING - 1], 1); // 75
  expect_cell(b2 -> map[2 * BLOCK_PADDING - 1][0], 1); // 76
  expect_cell(b2 -> map[BLOCK_SIZE][0], 1); // 77
  expect_cell(b2 -> map[BLOCK_SIZE + BLOCK_PADDING - 1][BLOCK_PADDING - 1], 1); // 78
  b2 -> map[BLOCK_PADDING][BLOCK_PADDING - 1] = 0;
  b2 -> map[2 * BLOCK_PADDING - 1][0] = 0;
  b2 -> map[BLOCK_SIZE][0] = 0;
  b2 -> map[BLOCK_SIZE + BLOCK_PADDING - 1][BLOCK_PADDING - 1] = 0;
  validate_block(b2); // 79
  empty_inside(b2); // 80
  delete b2;
}

void check_compression()
{
  uint64_t compressed;
  int i, j, ineg, jneg;
  set<uint64_t> unique_map;
  for(i = 1, ineg = 0; i; i <<= 1)
  {
    do
    {
      if(i > 0)//can be negated
      {
        ineg = 1;
        i *= -1;
      }
      else
      {
        if(ineg)
        {
          i *= -1;
        }
        ineg = 0;
      }
      for(j = 1, jneg = 0; j; j <<= 1)
      {
        do
        {
          if(j > 0)//can be negated
          {
            jneg = 1;
            j *= -1;
          }
          else
          {
            if(jneg)
            {
              j *= -1;
            }
            jneg = 0;
          }
          compressed = compress_xy(i,j);
          expect_int(get_x(compressed), i);
          expect_int(get_y(compressed), j);
          tests++;
          if(unique_map.find(compressed) != unique_map.end())
          {
            fails++;
            fprintf(stderr, "value %llu obtained from %d and %d not unique", compressed, i, j);
          }
          else
          {
            unique_map.insert(compressed);
          }
        } while (jneg);
      }
    } while (ineg);
  }
  unique_map.clear();
}

int main()
{
  b1 = new Block(0, 0);
  check_region_change_flag();
  check_sync();
  check_compression();
  fprintf(stderr, "%d/%d tests passed\n", tests - fails, tests);
  return fails;
}
