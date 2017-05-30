#ifndef TEST_HPP
#define TEST_HPP

#include "../src/all.hpp"

void correct_valid_coordonates(Block *b, int x, int y, int expected);
void correct_count_cell_neighbours(Block &b, int x, int y, int expected);
void correct_revive_cell(Block *b, int x, int y, CELL_TYPE expected);
void correct_crank_cell(Block *b, int x, int y, CELL_TYPE expected);
void correct_crank(Block *b, Block *expected);

#endif
