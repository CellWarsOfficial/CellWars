#include <players.hpp>
#include <cstdio>
#include <strings.hpp>
#include <string>
#include <cstring>
#include <exception>
#include <functional>

using namespace std;
// TODO: improve concurreny within file

#define ME "Player manager"

Player::Player(CELL_TYPE type)
{
  this -> type = type;
  message_id = CW_PROT_SV_MIN_SEQ;
  score = 0; // scrub
}

Player::~Player()
{
}

int Player::get_score()
{
  player_lock.lock();
  int ret_value = this -> score;
  player_lock.unlock();
  return ret_value;
}

void Player::set_score(int score)
{
  player_lock.lock();
  this -> score = score;
  player_lock.unlock();
}

void *Player::get_data()
{
  player_lock.lock();
  return this -> extensionData;
}

void Player::set_data(void *extensionData)
{
  this -> extensionData = extensionData;
  player_lock.unlock();
}

int Player::connect(Websocket_Con *ws)
{
  player_lock.lock();
  int ret_value = 1;
  if(this -> ws == NULL)
  {
    this -> ws = ws;
    ret_value = 0;
  }
  player_lock.unlock();
  return ret_value;
}

Websocket_Con *Player::disconnect()
{
  player_lock.lock();
  Websocket_Con *ret_value = ws;
  ws = NULL;
  player_lock.unlock();
  return ret_value;
}
  
int Player::get_next_message_id()
{
  player_lock.lock();
  if(message_id == CW_PROT_SV_MAX_SEQ)
  {
    message_id = CW_PROT_SV_MIN_SEQ;
  }
  else
  {
    message_id++;
  }
  int ret_value = message_id;
  player_lock.unlock();
  return ret_value;
}

void Player::send(string message)
{
  player_lock.lock();
  if(ws != NULL)
  {
    ws -> writews(message);
  }
  player_lock.unlock();
}

int Player::send_seq(string message)
{
  int seq = get_next_message_id();
  send(form(seq, message));
  return seq;
}

Player_Manager::Player_Manager(DB_conn *db, Logger *log)
{
  this -> log = log;
  database = db;
  cb = bind(&Player_Manager::handle_client_message, this, placeholders::_1, placeholders::_2);
  active_conns = 0;
}

Player_Manager::~Player_Manager()
{
  std::map<CELL_TYPE, Player *>::iterator i;
  manager_lock.lock();
  for (i = player_list.begin(); i != player_list.end(); i++)
  {
    delete i -> second;
  }
  player_list.clear();
  player_mapper.clear();
  manager_lock.unlock();
}

void Player_Manager::expand(Game *game)
{
  this -> game = game;
}

void Player_Manager::subscribe(Websocket_Con *ws)
{
  manager_lock.lock();
  active_conns++;
  manager_lock.unlock();
}

function<void(Websocket_Con *,string)> Player_Manager::get_callback()
{
  return cb;
}

void Player_Manager::kill(CELL_TYPE t)
{
  Player *p = get_player(t);
  manager_lock.lock();
  p -> send_seq("LOST");
  detach(p -> disconnect());
  manager_lock.unlock();
}

Player *Player_Manager::get_player(CELL_TYPE t)
{
  Player *ret_value = NULL;
  if(t == DEAD_CELL)
  {
    return ret_value;
  }
  map<CELL_TYPE, Player *>::iterator i;
  manager_lock.lock();
  i = player_list.find(t);
  if(i != player_list.end())
  {
    ret_value = i -> second;
  }
  manager_lock.unlock();
  return ret_value;
}

void Player_Manager::bcast_message(string message)
{
  log -> record(ME, "broadcasting message [" + message + "]");
  std::map<CELL_TYPE, Player *>::iterator i;
  manager_lock.lock();
  for (i = player_list.begin(); i != player_list.end(); i++)
  {
    i -> second -> send_seq(message);
  }
  manager_lock.unlock();
}

void Player_Manager::handle_client_message(Websocket_Con *ws, string msg)
{
  if(msg.length() == 0)
  {
    delete ws;
    manager_lock.lock();
    active_conns--;
    manager_lock.unlock();
    return;
  }
  log -> record(ME, msg);
  const char *key = msg.c_str();
  try
  {
    int seq_id = get_seq_id(key);
    key = string_seek(key, ":");
    check_not_null(key);
    if(seq_id < CW_PROT_SV_MIN_SEQ)
    {
      resolve_callback(ws, seq_id, key);
      return;
    }
    string method = get_method(key);
    if(method.compare("PICK") == 0)
    {
      resolve_pick(ws, seq_id, key);
      return;
    }
    if(method.compare("DETAILS") == 0)
    {
      resolve_details(ws, seq_id, key);
      return;
    }
    if(method.compare("UPDATE") == 0)
    {
      resolve_update(ws, seq_id, key);
      return;
    }
    if(method.compare("SCORE") == 0)
    {
      resolve_score(ws, seq_id, key);
      return;
    }
  }
  catch (...)
  { // Protocol violation of sorts!
    ws -> kill(); // the fact that the thread maintaining the websocket gets to call this is pretty dark >.>
  }
}

void Player_Manager::resolve_callback(Websocket_Con *ws, int seq_id, const char *key)
{
  fprintf(stderr, "can't deal with [%s]!", key);
  // idk man, TODO: maybe?
  check_not_null(NULL);
}

void Player_Manager::resolve_pick(Websocket_Con *ws, int seq_id, const char *key)
{
  if(find_owner(ws))
  { // you already picked
    ws -> writews(form(seq_id, "0"));
    return;
  }
  CELL_TYPE t = (CELL_TYPE) stoi(get_arg(key, "t"));
  if(t == 0)
  { // no undead 'round here!
    ws -> writews(form(seq_id, "0"));
    return;
  }
  if(get_player(t) == NULL)
  { // can pick
    Player *p = new Player(t);
    p -> connect(ws);
    manager_lock.lock();
    player_list[t] = p;
    manager_lock.unlock();
    ws -> writews(form(seq_id, "1"));
  }
  else
  { // taken
    ws -> writews(form(seq_id, "0"));
  }
}

void Player_Manager::resolve_details(Websocket_Con *ws, int seq_id, const char *key)
{
  ws -> writews(form(seq_id, form(form("gtc", game -> get_gtc(), "="), form("wait", game -> get_wait(), "="), " ")));
}

void Player_Manager::resolve_update(Websocket_Con *ws, int seq_id, const char *key)
{
  int px = stoi(get_arg(key, "px"));
  int py = stoi(get_arg(key, "py"));
  CELL_TYPE t = (CELL_TYPE) stoi(get_arg(key, "t"));
  Player *owner = find_owner(ws);
  check_not_null(owner);
  ws -> writews(form(seq_id, game -> user_does(px, py, t, owner)));
}

void Player_Manager::resolve_score(Websocket_Con *ws, int seq_id, const char *key)
{
  CELL_TYPE t = (CELL_TYPE) stoi(get_arg(key, "t"));
  Player *p = get_player(t);
  int score = (p == NULL) ? 0 : p -> get_score();
  ws -> writews(form(seq_id, score));
}

void Player_Manager::detach(Websocket_Con *ws)
{
  map<Websocket_Con *, CELL_TYPE>::iterator i;
  manager_lock.lock();
  i = player_mapper.find(ws);
  if(i != player_mapper.end())
  {
    player_mapper.erase(i);
  }
  manager_lock.unlock();
}

void Player_Manager::attach(Websocket_Con *ws, CELL_TYPE t)
{
  manager_lock.lock();
  player_mapper[ws] = t;
  manager_lock.unlock();
}

Player *Player_Manager::find_owner(Websocket_Con *ws)
{
  Player *ret_value = NULL;
  CELL_TYPE ret_cell = DEAD_CELL;
  map<Websocket_Con *, CELL_TYPE>::iterator i;
  manager_lock.lock();
  i = player_mapper.find(ws);
  if(i != player_mapper.end())
  {
    ret_cell = i -> second;
  }
  manager_lock.unlock();
  ret_value = get_player(ret_cell);
  return ret_value;
}

void check_not_null(const void *key)
{
  if(key == NULL)
  {
    throw invalid_argument("null");
  }
}

int get_seq_id(const char *source)
{
  return stoi(string_get_next_token(source, STR_WHITE));
}

string get_method(const char *source)
{
  return string_get_next_token(source, STR_WHITE);
}

string get_arg(const char *source, string target)
{
  return string_get_next_token(string_seek(source, target + "="), STR_WHITE);
}