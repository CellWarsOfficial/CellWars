#ifndef PLAYERS_HPP
#define PLAYERS_HPP
#include <map>
#include <mutex>
#include <functional>
#include <log.hpp> // for Logger
#include <game.hpp> // for Game
#include <database.hpp> // for DB_conn
#include <websocket.hpp> // for Websocket_Con
#include <constants.hpp> // for CELL_TYPE

class Game;
class Player_Manager;

using namespace std;

class Player
{
  public:
  Player(CELL_TYPE type);
  ~Player();
  int get_score();
  void set_score(int score);
  void *get_data();
  void set_data(void *extensionData);
  int connect(Websocket_Con *ws);
  Websocket_Con *disconnect();
  int get_next_message_id();
  void send(string message);
  int send_seq(string message);
  CELL_TYPE type;
  void *extensionData;
  private:
  int message_id;
  int score;
  Websocket_Con *ws;
  mutex player_lock;
};

class Player_Manager
{
  public:
  Player_Manager(DB_conn *db, Logger *log);
  ~Player_Manager();
  void expand(Game *game);
  void subscribe(Websocket_Con *ws);
  function<void(Websocket_Con *, string)> get_callback();
  void kill(CELL_TYPE t);
  void bcast_message(string message);
  private:
  void handle_client_message(Websocket_Con *ws,string msg);
  void resolve_callback(Websocket_Con *ws, int seq_id, const char *key);
  void resolve_pick(Websocket_Con *ws, int seq_id, const char *key);
  void resolve_details(Websocket_Con *ws, int seq_id, const char *key);
  void resolve_update(Websocket_Con *ws, int seq_id, const char *key);
  void resolve_score(Websocket_Con *ws, int seq_id, const char *key);
  Player *find_owner(Websocket_Con *ws);
  function<void(Websocket_Con *, string)> cb;
  map <CELL_TYPE, Player*> player_list;
  Logger *log;
  DB_conn *database;
  Game *game;
};

#endif

void check_not_null(const void *key); // throws

int get_seq_id(const char *source); // throws

string get_method(const char *source);

string get_arg(const char *source, string target);