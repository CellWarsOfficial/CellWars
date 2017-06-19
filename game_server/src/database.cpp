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
  q_count = 0;
  safe = 1;
  address = a;
  log = l;
  socketid = socket(AF_INET, SOCK_STREAM, 0);
  struct hostent *server;
  struct sockaddr_in server_address;
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
  answer_buf = new char[DB_MAX_BUF + BUF_PROCESS_ERROR];
  memset(answer_buf, 0, (DB_MAX_BUF + BUF_PROCESS_ERROR) * sizeof(char));
  check_malloc(answer_buf);
  log -> record(ME, "Connection successful");
}

DB_conn::~DB_conn()
{
  close(socketid);
  delete[] answer_buf;
}

string DB_conn::run_query(int expectation, string s)
{
  log -> record(ME, (string)"Running query " + s);
  string wrapper;
  int len, aux, tlen = 0;
  if(expectation)
  {
    wrapper = "copy (SELECT count(*) FROM agents.grid";
    const char *point = string_seek(s.c_str(), "WHERE");
    if(point)
    {
      wrapper = wrapper + " WHERE " + string_get_next_token(point, "");
    }
    wrapper = wrapper + ") to STDOUT DELIMITER ' '; copy (" + s 
              + ") TO STDOUT DELIMITER ' '; \\echo #\n";
  }
  else
  {
    wrapper = s + "; \\echo #\n";
  }
  const char *c = wrapper.c_str();

  db_lock.lock();
  string result = "";
  q_count++;
  len = write(socketid, c, strlen(c));
  if(len < 0)
  {
    fprintf(stderr, "DB connection broken, PANIC!\n");
    exit(0);
  }
  len = read(socketid, answer_buf, DB_MAX_BUF - 1);
  if(len < 0)
  {
    fprintf(stderr, "DB connection broken, PANIC!\n");
    exit(0);
  }
  while(string_seek(answer_buf, "#") == 0)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(DB_DELAY));
    if(DB_MAX_BUF - len < BUF_THRESHOLD)
    { // refresh the buffer
      result = result + string_get_next_token(answer_buf, "#");
      memset(answer_buf, 0, sizeof(char) * len);
      tlen += len;
      len = 0;
    }
    aux = read(socketid, answer_buf + len, DB_MAX_BUF - len);
    if(aux < 0)
    {
      fprintf(stderr, "DB connection broken, PANIC!\n");
      exit(0);
    }
    len = len + aux;
  }
  tlen += len;
  log -> record(ME, (string)"Received " + to_string(tlen));
  result = result + string_get_next_token(answer_buf, "#");
  memset(answer_buf, 0, sizeof(char) * len);
  db_lock.unlock();
  return result;
}

void DB_conn::insert_query_builder(CELL_TYPE t, int x, int y)
{
  if((t == 0) && (size != 0))
  { // t = 0 means flush whatever you have
    run_query(NO_READ, constructed_query);
    size = 0;
    return;
  }
  if(size == 0)
  {
    constructed_query = "INSERT INTO agents.grid(user_id, x, y) VALUES ("
        + to_string(t) + ", "
        + to_string(x) + ", "
        + to_string(y) + ")";
    size++;
  }
  else
  {
    constructed_query = constructed_query + ", ("
        + to_string(t) + ", "
        + to_string(x) + ", "
        + to_string(y) + ")";
    size++;
    if(size == DB_MAX_BUILD_SIZE)
    {
      run_query(NO_READ, constructed_query);
      size = 0;
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
    insert_query_builder(c, x, y);
  }
  insert_query_builder(0, 0, 0);
  fclose(file);
}

void DB_conn::demand_stat()
{
  log -> record(ME, (string)"Connected to " + address);
  log -> record(ME, "Sent " + to_string(q_count) + " since starting");
}
