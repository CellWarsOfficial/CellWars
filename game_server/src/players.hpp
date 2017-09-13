#ifndef PLAYERS_HPP
#define PLAYERS_HPP
#include <map>
#include <mutex>
#include <functional>
#include <game.hpp>
#include <websocket.hpp>
#include <log.hpp>
#include <constants.hpp>

class game;
class Player_Manager;

using namespace std;

class Player{
  public:
  Player(CELL_TYPE type);
  ~Player();
  void lock();
  void unlock();
  CELL_TYPE type;
  void *extensionData;
  private:
  mutex player_lock;
};

class Player_Manager{
  public:
  Player_Manager(Game *game, Logger *log);
  ~Player_Manager();
  void subscribe(Websocket_Con *ws);
  function<void(Websocket_Con *,string)> get_callback();
  private:
  void handle_client_message(Websocket_Con *ws,string msg);
  function<void(Websocket_Con *,string)> cb;
  map <int, Player*> playerList;
  Logger *log;
  Game *game;
};

#endif