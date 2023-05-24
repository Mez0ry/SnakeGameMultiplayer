#ifndef SnakeGame_MAP_HPP
#define SnakeGame_MAP_HPP
#include <memory>
#include <iostream>
#include "Entity.hpp"
#include "../Shared/Vector.hpp"
#include <ncurses.h>

#define MAP_ROWS 20
#define MAP_COLS 20

class Map {
public:
  Map();
  ~Map();

  void Update();

  [[nodiscard]]  uint8_t** GetMap(){
    return m_pMap;
  }

  static void RenderEntity(Map& map,Entity& entity){
    auto& pos = entity.GetPosition();
    if(IsOnBoard(pos)){
      move(pos.y,pos.x);
      addch(entity.GetSymbol());
      map.GetMap()[pos.y][pos.x] = entity.GetSymbol();
    }
  }

  void Render();

private:
  void CleanMap();
  void FillMap();

  static bool IsOnBoard(const Vec2& pos){
    return !(pos.x <= 0 || pos.y <= 0 || pos.x > (MAP_COLS - 1) || pos.y > (MAP_ROWS - 1));
  }
  
private:
  uint8_t **m_pMap;
};

#endif //! SnakeGame_MAP_HPP