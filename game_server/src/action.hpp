#ifndef ACTION_HPP
#define ACTION_HPP

#include <block.hpp>
#include <constants.hpp>
#include <bitset>

class Action
{
public:
  virtual ~Action() = default;
  virtual void crank(Block *block) = 0;
  virtual void crank_for(Block *block, int generations) = 0;
  virtual CELL_TYPE crank_cell(Block *block, int x, int y) = 0;
  virtual CELL_TYPE revive_cell(Block *block, int x, int y) = 0;
  virtual int count_cell_neighbours(Block *block, int x, int y) = 0;
};

#endif
