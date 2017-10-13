#ifndef DATABASE_H
#define DATABASE_H

#include <websocket.hpp>
#include <builder.hpp>
#include <log.hpp>
#include <cstdio>
#include <string>
#include <mutex>
#include <map>

using namespace std;

class Database
{
  public:
  Database(Websocket_Con *ws, string con_string);
  ~Database();
  void send(string message);
  string get_con_string();
  void set_flag(int value);
  void clear_flag(int value);
  bool is_safe();
  int load;
  private:
  Websocket_Con *ws;
  int flags;
  string con_string;
  mutex database_lock;
};

class DB_conn
{
  public:
  DB_conn(Logger *log);
  ~DB_conn();
  void subscribe(Websocket_Con *ws);
  function<void(Websocket_Con *, string)> get_callback();
  string get_db_for_client();
  void erase();
  void swap();
  void put(CELL_TYPE t, int x, int y);
  void flush();
  void set_variable(string variable_name, string value);
  void bcast_message(string message);
  void demand_stat();
  private:
  void bcast_message(string message, int flag);
  string generate_msg(string action);
  string generate_msg(string action, string data);
  void handle_database_message(Websocket_Con *ws, string msg);
  function<void(Websocket_Con *, string)> cb;
  Query_builder *query_builder;
  map <Websocket_Con *, Database *> database_list;
  map <string, string> variables;
  Logger *log;
  int erase_status;
  int active_conns;
  int avg_load;
  int queries;
  mutex manager_lock;
};
#endif
