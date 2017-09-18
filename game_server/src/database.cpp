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

#define ME "Database"
Database::Database(Websocket_Con *ws, string con_string)
{
  this -> ws = ws; // deleted by senior
  this -> con_string = con_string;
  load = 0;
  flags = 0;
}

Database::~Database()
{
}

void Database::send(string message)
{
  ws -> writews(message);
}

string Database::get_con_string()
{
  return con_string;
}

void Database::set_flag(int value)
{
  flags = flags | value;
}

void Database::clear_flag(int value)
{
  flags = flags & ~value;
}

bool Database::is_safe()
{
  return flags & 3; // a database is considered safe if it has been erased twice
}

DB_conn::DB_conn(Logger *log)
{
  this -> log = log;
  cb = bind(&DB_conn::handle_database_message, this, placeholders::_1, placeholders::_2);
  erase_status = 1;
  active_conns = 0;
  avg_load = 0;
}

DB_conn::~DB_conn()
{
  std::map<Websocket_Con *, Database *>::iterator i;
  manager_lock.lock();
  for (i = database_list.begin(); i != database_list.end(); i++)
  {
    delete i -> second;
  }
  database_list.clear();
  manager_lock.unlock();
}

void DB_conn::subscribe(Websocket_Con *ws)
{
  manager_lock.lock();
  active_conns++;
  manager_lock.unlock();
}

function<void(Websocket_Con *, string)> DB_conn::get_callback()
{
  return cb;
}

string DB_conn::get_db_for_client()
{
  std::map<Websocket_Con *, Database *>::iterator i;
  string ret_value = "";
  Database *db;
  manager_lock.lock();
  for (i = database_list.begin(); i != database_list.end(); i++)
  {
    db = i -> second;
    ret_value = db -> get_con_string();
    if((db -> is_safe()) && (db -> load <= avg_load))
    {
      break;
    }
  }
  manager_lock.unlock();
  return ret_value;
}

void DB_conn::erase()
{
  bcast_message("ERASE", erase_status);
  erase_status = erase_status % 2 + 1; // alternates between 2 and 1
}

void DB_conn::swap()
{
  bcast_message("SWAP");
}

void DB_conn::put(CELL_TYPE t, int x, int y)
{
  bcast_message("SET (" + to_string(x) + ", " + to_string(y) + ") = " + to_string(t));
}

void DB_conn::set_variable(string variable_name, string value)
{
  manager_lock.lock();
  variables[variable_name] = value;
  manager_lock.unlock();
}

void DB_conn::bcast_message(string message)
{
  return bcast_message(message, 0);
}

void DB_conn::bcast_message(string message, int flag)
{
  log -> record(ME, "broadcasting [" + message + "]");
  std::map<Websocket_Con *, Database *>::iterator i;
  manager_lock.lock();
  for (i = database_list.begin(); i != database_list.end(); i++)
  {
    i -> second -> send(message);
    i -> second -> set_flag(flag);
  }
  manager_lock.unlock();
}

void DB_conn::handle_database_message(Websocket_Con *ws, string msg)
{
  if(msg.length() == 0)
  {
    manager_lock.lock();
    database_list.erase(ws);
    active_conns--;
    manager_lock.unlock();
    delete ws;
    return;
  }
  log -> record(ME, msg);
}

void DB_conn::demand_stat()
{
  log -> record(ME, "not implemented :(");
}
