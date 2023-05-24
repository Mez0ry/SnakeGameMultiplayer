#ifndef SnakeGame_GAME_HPP
#define SnakeGame_GAME_HPP
#include "Map.hpp"
#include "Snake.hpp"
#include "../Shared/ENetWrapper/ENetWrapper.hpp"
#include <sstream>
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
#include "GameScore.hpp"
#include "Serializer.hpp"

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
  ClientData m_ClientData;
  Window m_Window;
private:
  bool m_bRunning;
  Map m_Map;

  std::unordered_map<uint32_t,Snake> m_Snakes;
  std::unordered_map<uint32_t,Food> m_FoodMap;
  std::unordered_map<uint32_t,GameScore> m_GameScoreMap;
  std::unordered_map<uint32_t,std::string> m_UserNameMap;

  ENet::ClientWrapper m_ClientWrapper;
  uint8_t m_key;
};
#endif //! SnakeGame_GAME_HPP