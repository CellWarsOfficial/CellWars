#include <database.hpp>
#include <block.hpp>
#include <constants.hpp>
#include <cstdio>
#include <cstdlib>
#include <string>

// TODO all functions in here.

#define ME "Database"

void *init_db(const char *db, Logger *log)
{
  log -> record(ME, (string)"Connection to database \"" + db + "\" successful");
  void* db_info = (void*)new int; // duck typing
  return db_info;
}

void *run_query(string s)
{
  return NULL;
}

Block **load_from_db(long NW, long SE)
{
  int NWx = get_x(NW), NWy = get_y(NW), SEx = get_x(SE), SEy = get_y(SE);
  int minx, miny, maxx, maxy;
  if((NWx > SEx) || (NWy > SEy))
  {
    return (Block**) new void*(NULL);
  }
  if(NWx <= 0)
  {
    for(minx = 0; minx > NWx; minx -= BLOCK_SIZE);
  }
  else
  {
    for(minx = 0; minx + BLOCK_SIZE <= NWx; minx += BLOCK_SIZE);
  }
  if(NWy <= 0)
  {
    for(miny = 0; miny > NWy; miny -= BLOCK_SIZE);
  }
  else
  {
    for(miny = 0; miny + BLOCK_SIZE <= NWy; miny += BLOCK_SIZE);
  }
  for(maxx = minx; maxx + BLOCK_SIZE <= SEx; maxx += BLOCK_SIZE);
  for(maxy = miny; maxy + BLOCK_SIZE <= SEy; maxy += BLOCK_SIZE);
  Block **blocks_to_return = new Block*[
    ((maxx - minx) / BLOCK_SIZE + 1) * ((maxy - miny) / BLOCK_SIZE + 1) + 1
    ];
  int block_id = 0;
  for(int px = minx; px <= maxx; px++)
  {
    for(int py = miny; py <= maxx; py++, block_id++)
    {
      blocks_to_return[block_id] = new Block(px, py);
      run_query("SELECT * FROM cell_info WHERE row>="
        + to_string(minx - BLOCK_PADDING) + " AND row<"
        + to_string(minx + BLOCK_SIZE + BLOCK_PADDING) + " AND col>="
        + to_string(miny - BLOCK_PADDING) + " AND col<"
        + to_string(miny + BLOCK_SIZE + BLOCK_PADDING)
        );
      // TODO place returned information inside block
    }
  }
  blocks_to_return[block_id] = NULL; // Sentinel marking the end of the list
  return blocks_to_return;
}

void update_db(Block *block)
{
  int i, j;
  run_query("DELETE FROM cell_info WHERE row>="
    + to_string(block -> originx) + " AND row<"
    + to_string(block -> originx + BLOCK_SIZE) + " AND col>="
    + to_string(block -> originy) + " AND col<"
    + to_string(block -> originy + BLOCK_SIZE));
  for(i = BLOCK_PADDING; i < BLOCK_PADDING + BLOCK_SIZE; i++)
  {
    for(j = BLOCK_PADDING; j < BLOCK_PADDING + BLOCK_SIZE; j++)
    {
      run_query("INSERT INTO cell_info(type, row, col) VALUES ("
        + to_string(block -> map[i][j]) + ", "
        + to_string(i + block -> originx - BLOCK_PADDING) + ", "
        + to_string(j + block -> originy - BLOCK_PADDING) + ")"
        );
    }
  }
}
