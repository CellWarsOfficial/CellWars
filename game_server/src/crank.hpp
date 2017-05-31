#ifndef CRANK_HPP
#define CRANK_HPP

#include <block.hpp>
#include <constants.hpp>
#include <bitset>

void crank(Block *block);
CELL_TYPE crank_cell(Block *block, int x, int y);
CELL_TYPE revive_cell(Block *block, int x, int y);
int count_cell_neighbours(Block *block, int x, int y);
int valid_coordonate(int x, int y);
int equals(Block *current, Block *other);
void init_bitmap(Block *block);
int prune_area(Block *block, int x, int y);

#endif