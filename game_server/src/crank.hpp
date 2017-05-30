#ifndef CRANK_HPP
#define CRANK_HPP

#include "block.hpp"
#include "constants.hpp"

Block *crank(Block *block);
void crank_cell(Block *block, int x, int y);
void revive_cell(Block *block, int x, int y);
int count_cell_neighbours(Block *block, int x, int y);
int valid_coordonate(int x, int y);
int equals(Block *current, Block *other);

#endif
