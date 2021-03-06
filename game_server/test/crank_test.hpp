#ifndef CRANK_TEST_HPP
#define CRANK_TEST_HPP

#include <crank.hpp>

void correct_valid_coordonates(Block *b, Action *a, int x, int y, int expected);
void correct_count_cell_neighbours(Block *b, Action *a, int x, int y, int expected);
void correct_revive_cell(Block *b, Action *a, int x, int y, CELL_TYPE expected);
void correct_crank_cell(Block *b, Action *a, int x, int y, CELL_TYPE expected);
void correct_crank(Block *b, Block *expected, Action *a);

#endif
