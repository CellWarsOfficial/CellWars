#include <players.hpp>
#include <cstdio>
#include <strings.hpp>
#include <string>
#include <cstring>
#include <exception>
#include <functional>

using namespace std;

Player::Player(CELL_TYPE type)
{
  this.type = type;
  message_id = CW_PROT_SV_MIN_SEQ;
}

Player::~Player()
{
}

int Player::get_score()
{
  player_lock.lock();
  int ret_value = this -> score;
  player_lock.unlock();
  return this.score;
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

void Player::set_data(void *extensionData);
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

void Player_Manager::kill(CELL_TYPE t)
{
  Player *p = player_list[t];
  p -> send_seq("LOST");
}

void bcast_message(string message)
{ 
  std::map<CELL_TYPE, Player *>::iterator i;
  for (i = player_list.begin(); i != player_list.end(); i++)
  {
    i -> second -> send_seq(message);
  }
}

void Player_Manager::handle_client_message(Websocket_Con *ws, string msg)
{
  if(msg == "")
  {
    delete ws;
    return
  }
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

void Player_Manager::resolve_callback(Websocket_Con *ws, int seq_id, const chat *key)
{
  // idk man, TODO: maybe?
  check_not_null(0);
}

void Player_Manager::resolve_pick(Websocket_Con *ws, int seq_id, const chat *key)
{
  CELL_TYPE t = (CELL_TYPE) stoi(get_arg(key, "t="));
  if(t == 0)
  { // no undead 'round here!
    ws -> writews(form(seq_id, "0"));
  }
  if(player_list[t] == NULL)
  { // can pick
    player_list[t] = new Player(t);
    player_list[t].connect(ws);
    ws -> writews(form(seq_id, "1"));
  }
  else
  { // taken
    ws -> writews(form(seq_id, "0"));
  }
}

void Player_Manager::resolve_details(Websocket_Con *ws, int seq_id, const chat *key)
{
  ws -> writews(form(seq_id, form(form("gtc", game -> get_gtc(), "="), form("wait", game -> get_wait(), "="), " ")));
}

void Player_Manager::resolve_update(Websocket_Con *ws, int seq_id, const chat *key)
{
  int px = stoi(get_arg(key, "px="));
  int py = stoi(get_arg(key, "py="));
  CELL_TYPE t = (CELL_TYPE) stoi(get_arg(key, "t="));
  Player *owner = find_owner(ws);
  check_not_null(owner)
  ws -> writews(form(seq_id, game -> user_does(px, py, t, owner -> type)));
}

void Player_Manager::resolve_score(Websocket_Con *ws, int seq_id, const chat *key)
{
  CELL_TYPE t = (CELL_TYPE) stoi(get_arg(key, "t="));
  int score = (player_list[t] == NULL) ? 0 : player_list[t] -> get_score();
  ws -> writews(form(seq_id, score));
}

Player *Player_Manager::find_owner(Websocket_Con *ws)
{
  return NULL; // TODO: fix zis
}

void check_not_null(void *key)
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
  return string_get_next_token(string_seek(source, target), STR_WHITE);
}

string form(string seq, string message, string sep)
{
  return seq + sep + message;
}

string form(int seq, string message, string sep)
{
  return form(to_string(seq), message, string sep);
}

string form(string seq, int message, string sep)
{
  return form(seq, to_string(message), string sep);
}

string form(int seq, int message, string sep)
{
  return form(to_string(seq), to_string(message), string sep);
}

string form(string seq, string message)
{
  return form(seq, message, ": ");
}

string form(int seq, string message)
{
  return form(to_string(seq), message);
}

string form(string seq, int message)
{
  return form(seq, to_string(message));
}

string form(int seq, int message)
{
  return form(to_string(seq), to_string(message));
}