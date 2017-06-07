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
  virtual CELL_TYPE crank_cell(Block *block, int x, int y) = 0;
  virtual CELL_TYPE revive_cell(Block *block, int x, int y) = 0;
  virtual int valid_coordonate(int x, int y) = 0;
  virtual int count_cell_neighbours(Block *block, int x, int y) = 0;
  virtual int equals(Block *current, Block *other) = 0;
  virtual int prune_area(Block *block, int x, int y) = 0;
  virtual void inc_area_count(Block *block, int x, int y) = 0;
  virtual void dec_area_count(Block *block, int x, int y) = 0;
  virtual int get_area_count(int x, int y, CELL_TYPE before, CELL_TYPE after) = 0;
  virtual void update_area_count(Block *block, int x, int y, int c) = 0;
};

#endif
