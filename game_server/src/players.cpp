#include <players.hpp>
#include <cstdio>

Player::Player(CELL_TYPE type)
{
  this.type = type;
}

Player::~Player()
{
}

void Player::lock()
{
  player_lock.lock();
}

void Player::unlock()
{
  player_lock.unlock();
}

Player_Manager::Player_Manager(Game *game, Logger *l)
{
  this -> game = game;
  this -> log = log;
  cb = bind(Player_Manager::handle_client_message, this, placeholders::_1, placeholders::_2);
}
Player_Manager::~Player_Manager()
{
}

void Player_Manager::subscribe(Websocket_Con *ws)
{
}

function<void(Websocket_Con *,string)> Player_Manager::get_callback()
{
  return cb;
}

Player_Manager::handle_client_message(Websocket_Con *ws,string msg)
{
  printf("%s\n", msg.c_str());
}