#include <database.hpp>
#include <block.hpp>
#include <constants.hpp>
#include <malloc_safety.hpp>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <all.hpp> 

// TODO all functions in here.

#define ME "Database"

DB_conn::DB_conn(const char *a, Logger *l)
{
  safe = 1;
  address = a;
  log = l;
  socketid = socket(AF_INET, SOCK_STREAM, 0);
  struct hostent *server;
  struct sockaddr_in server_address;
/*
  sio::client c;
  log -> record(ME, string(a) + ":" + DB_PORT);
  c.connect(string(a) + ":" +DB_PORT);
  c.socket()->emit("login");
  return;
 */
  if(socketid < 0)
  {
    safe = 0;
    log -> record(ME, "Failed to initialise socket");
    return;
  }
  server = gethostbyname(address);
  if(server == NULL)
  {
    safe = 0;
    log -> record(ME, "Failed to detect database");
    return;
  }

  bzero((char *) &server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
       (char *)&server_address.sin_addr.s_addr,
       server->h_length);
  server_address.sin_port = htons(DB_PORT);
  if(connect(
     socketid, 
     (struct sockaddr *)&server_address, 
     sizeof(server_address)
     ) < 0)
  {
    safe = 0;
    log -> record(ME, "Failed to connect to database");
    return;
  }
  log -> record(ME, "Connection successful");
}

void *DB_conn::run_query(int expectation, string s)
{
  string wrapper;
  if(expectation)
  {
    wrapper = "copy (SELECT count(*) FROM agents.grid) to STDOUT WITH CSV; copy (" + s + ") TO STDOUT WITH CSV;\n";
  }
  else
  {
    wrapper = s + ";\n";
  }
  const char *c = wrapper.c_str();
  char answer_buf[DB_MAX_BUF];
  log -> record(ME, (string)"Running query " + wrapper);
  //
  write(socketid, c, strlen(c));

  //c.socket()->emit("benis");
  //
  if(expectation)
  {
    //struct answer *result = 0;
    bzero(answer_buf, DB_MAX_BUF);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    while(read(socketid, answer_buf, DB_MAX_BUF - 1) == 0)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    if(expectation == EXPECT_CLIENT)
    {
      //TODO: free ans string
      string *ans = new string();
      for(int i = 0; answer_buf[i]; i++)
      {
        printf("add %c | %d\n", answer_buf[i], i);
        *ans = *ans + answer_buf[i];
      }
      log->record("tetris", *ans);
      log->record("tetris_done", "ye");
      return (void*) ans;
    }/*
    int ni = 0;
    int number[3] = {0, 0, 0};
    int neg = 1;
    int i = 0;
    for(; (answer_buf[i] >= '0') && (answer_buf[i] <= '9'); i++);
    for(; !((answer_buf[i] >= '0') && (answer_buf[i] <= '9')); i++);
    for(; answer_buf[i]; i++)
    {
      if(answer_buf[i] == '-')
      {
        neg = -1;
        continue;
      }
      if((answer_buf[i] >= '0') && (answer_buf[i] <= '9'))
      {
        number[ni] = number[ni] * 10 + (int)(answer_buf[i] - '0');
        continue;
      }
      number[ni] = number[ni] * neg;
      neg = 1;
      ni++;
      if(ni == 3)
      {
        void *aux = malloc(sizeof(struct answer));
        ((struct answer *)aux) -> next = result;
        result = (struct answer *)aux;
        result -> row = number[0];
        result -> col = number[1];
        result -> t = number[2];
        number[0] = 0;
        number[1] = 0;
        number[2] = 0;
        ni = 0;
      }
    }
    return (void *)result;*/
    return NULL;
  }
  return NULL;
}

Block **DB_conn::load_from_db(uint64_t NW, uint64_t SE)
{
  int NWx = get_x(NW) - BLOCK_PADDING, NWy = get_y(NW) - BLOCK_PADDING;
  int SEx = get_x(SE) + BLOCK_PADDING, SEy = get_y(SE) + BLOCK_PADDING;
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
  for(int px = minx; px <= maxx; px += BLOCK_SIZE)
  {
    for(int py = miny; py <= maxx; py += BLOCK_SIZE, block_id++)
    {
      Block* blk = new Block(px, py);
      struct answer *a, *b;
      a = (struct answer *)run_query(EXPECT_READ, 
        "SELECT * FROM agents.grid WHERE x>="
        + to_string(px - BLOCK_PADDING) + " AND x<"
        + to_string(px + BLOCK_SIZE + BLOCK_PADDING) + " AND y>="
        + to_string(py - BLOCK_PADDING) + " AND y<"
        + to_string(py + BLOCK_SIZE + BLOCK_PADDING)
        );
      // TODO place returned information inside block
      while(a)
      {
        blk -> map[blk -> rectify_x(a -> row)][blk -> rectify_y(a -> col)] = a -> t;
        b = a;
        a = a -> next;
        free((void*) b);
      }
      blocks_to_return[block_id] = blk;
    }
  }
  blocks_to_return[block_id] = NULL; // Sentinel marking the end of the list
  return blocks_to_return;
}

void DB_conn::update_db(Block *block)
{
  int i, j;
  run_query(NO_READ, "DELETE FROM agents.grid WHERE x>="
    + to_string(block -> originx) + " AND x<"
    + to_string(block -> originx + BLOCK_SIZE) + " AND y>="
    + to_string(block -> originy) + " AND y<"
    + to_string(block -> originy + BLOCK_SIZE));
  for(i = BLOCK_PADDING; i < BLOCK_PADDING + BLOCK_SIZE; i++)
  {
    for(j = BLOCK_PADDING; j < BLOCK_PADDING + BLOCK_SIZE; j++)
    {
      if(block -> map[i][j])
      {
        run_query(NO_READ, "INSERT INTO agents.grid(user_id, x, y) VALUES ("
          + to_string(block -> map[i][j]) + ", "
          + to_string(i + block -> originx - BLOCK_PADDING) + ", "
          + to_string(j + block -> originy - BLOCK_PADDING) + ")"
          );
      }
    }
  }
}

void DB_conn::rewrite_db(const char *f)
{
  FILE *file = fopen(f, "r");
  check_malloc(file);
  if(file == 0)
  {
    return;
  }
  log -> record(ME, (string)"Loading database with values defined in " + f);
  run_query(NO_READ, "DELETE FROM agents.grid");
  int n, x, y, i, k;
  CELL_TYPE c;
  fscanf(file, "%d", &n);
  log -> record(ME, (string)"Loading #values = " + to_string(n));
  for(i = 1; i <= n; i++)
  {
    fscanf(file, "%d %d %d", &x, &y, &k);
    log -> record(ME, (string)"Loading " + to_string(x) + ' ' + to_string(y) + ' ' + to_string(k));
    c = CELL_TYPE(k);
    run_query(NO_READ, "INSERT INTO agents.grid(user_id, x, y) VALUES ("
      + to_string(c) + ", "
      + to_string(x) + ", "
      + to_string(y) + ")"
      );
  }
  fclose(file);
}
