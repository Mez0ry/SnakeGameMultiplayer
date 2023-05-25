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
  void InputHandler();
  void Update();
  void Render();
private:

 auto GetClientById(uint32_t id){
  auto it = std::find_if(m_ClientsData.begin(),m_ClientsData.end(),[&](const std::shared_ptr<ClientData>& client){
    return (client->id == id);
  });

  return (it != m_ClientsData.end()) ? (*it) : nullptr;
 }

private:
  Window m_Window;
private:
  bool m_bRunning;
  Map m_Map;

  std::unordered_map<uint32_t,Snake> m_Snakes;
  std::unordered_map<uint32_t,Food> m_FoodMap;
  //std::unordered_map<uint32_t,GameScore> m_GameScoreMap;
  
  std::vector<std::shared_ptr<ClientData>> m_ClientsData;

  ENet::ClientWrapper m_ClientWrapper;
  uint8_t m_key;
};
#endif //! SnakeGame_GAME_HPP