#ifndef SnakeGame_GAME_HPP
#define SnakeGame_GAME_HPP
#include "Map.hpp"
#include "Snake.hpp"
#include "../Shared/ENetWrapper/ENetWrapper.hpp"
#include <cstring>
#include <charconv>
#include "../Shared/Events.hpp"
#include "../Shared/Packet.hpp"
#include "../Shared/ClientData.hpp"
#include <unordered_map>
#include <ncurses.h>
#include "Window.hpp"
#include <chrono>
#include "Food.hpp"
#include "Serializer.hpp"
#include "../Shared/Stats.hpp"
#include <memory>

#define  GM_KEY_LEFT   4
#define  GM_KEY_RIGHT  5
#define  GM_KEY_UP     3
#define  GM_KEY_DOWN   2

class Game {
private:
  uint32_t m_fps = 60;
  float m_frameDelay = 2500 / m_fps;
public:
  Game();
  ~Game();

  void Play();

private:
  void HandleInput();
  void Update();
  void Render();
private:

 auto GetClientById(uint32_t id){
  auto it = std::find_if(m_ClientsData.begin(),m_ClientsData.end(),[&](const std::shared_ptr<ClientData>& client){
    return (client->id == id);
  });

  return (it != m_ClientsData.end()) ? (*it) : nullptr;
 }

 Vec2 GetDirection(uint8_t key){
  switch(key){
    case GM_KEY_LEFT:{
      return Vec2(-1,0);
      break;
    }
    case GM_KEY_RIGHT:{
      return Vec2(1,0);
      break;
    }
    case GM_KEY_UP:{
      return Vec2(0,-1);
      break;
    }
    case GM_KEY_DOWN:{
      return Vec2(0,1);
      break;
    }
  }
  return Vec2(0,0);
 }

 void RenderClientData(const std::shared_ptr<ClientData>& client_data, int offset_y){
  int win_w{m_Window.GetWindowWidth()},win_h{m_Window.GetWindowHeight()};

   std::string score_str = std::to_string(client_data->score);
   auto username = client_data->sUsername;
   auto wins = std::to_string(client_data->stats.wins);
   auto losses = std::to_string(client_data->stats.losses);
   
   std::string msg("Username: " + username + " -"  " Score: " + score_str + "," + " Wins: " + wins + "," + " Losses: " + losses + ".");
   move((win_h / 2) + offset_y,(win_w * 0.6) - (msg.length() / 2));
   printw(msg.c_str());
  
 }

private:
  Window m_Window;
  ENet::ClientWrapper m_ClientWrapper;
private:
  uint8_t m_LastPressedKey;
  bool m_bRunning;
  Map m_Map;

  std::unordered_map<uint32_t,Snake> m_Snakes;
  std::unordered_map<uint32_t,Food> m_FoodMap;
  
  std::vector<std::shared_ptr<ClientData>> m_ClientsData;

};
#endif //! SnakeGame_GAME_HPP