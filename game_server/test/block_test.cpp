#include "block_test.hpp"
#include <cstring>
#include <cstdio>

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

void check_region_change_flag()
{
  // NW
  b1 -> set(0, 0, 1);
  b1 -> set(BLOCK_PADDING - 1, BLOCK_PADDING - 1, 1);
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
  b1 -> set(2 * BLOCK_PADDING, 2 * BLOCK_PADDING, 1);
  expect_clean_flags(b1); // 10
  reset_region_flags(b1);
  // SE
  b1 -> set(BLOCK_FULL - 1, BLOCK_FULL - 1, 1);
  b1 -> set(BLOCK_SIZE + BLOCK_PADDING, BLOCK_SIZE + BLOCK_PADDING, 1);
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
  b1 -> set(BLOCK_SIZE - 1, BLOCK_SIZE - 1, 1);
  expect_clean_flags(b1); // 20
  reset_region_flags(b1);
  // NE
  b1 -> set(BLOCK_PADDING - 1, BLOCK_FULL - 1, 1);
  b1 -> set(BLOCK_SIZE + BLOCK_PADDING, BLOCK_SIZE + BLOCK_PADDING, 1);
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
  b1 -> set(2 * BLOCK_PADDING, BLOCK_SIZE - 1, 1);
  expect_clean_flags(b1); // 30
  reset_region_flags(b1);
  // SW
  b1 -> set(BLOCK_FULL - 1, BLOCK_PADDING - 1, 1);
  b1 -> set(BLOCK_SIZE + BLOCK_PADDING, BLOCK_SIZE + BLOCK_PADDING, 1);
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
  b1 -> set(BLOCK_SIZE - 1, 2 * BLOCK_PADDING, 1);
  expect_clean_flags(b1); // 40
  reset_region_flags(b1);
}

int main()
{
  b1 = new Block(0, 0);
  check_region_change_flag();
  fprintf(stderr, "%d/%d tests passed\n", tests - fails, tests);
  return fails;
}
