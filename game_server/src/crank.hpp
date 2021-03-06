#ifndef CRANK_HPP
#define CRANK_HPP

#include <block.hpp>
#include <action.hpp>
#include <constants.hpp>
#include <bitset>

class Crank: public Action
{
public:
virtual ~Crank() = default;
virtual void crank(Block *block);
virtual void crank_for(Block *block, int generations);
virtual CELL_TYPE crank_cell(Block *block, int x, int y);
virtual CELL_TYPE revive_cell(Block *block, int x, int y);
virtual int count_cell_neighbours(Block *block, int x, int y);
void init_bitmap(Block *block);
int prune_area(Block *block, int x, int y);
};
#endif
