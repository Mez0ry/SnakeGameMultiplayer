#ifndef SnakeGame_MAP_HPP
#define SnakeGame_MAP_HPP
#include <memory>
#include <iostream>
#include "Entity.hpp"
#include "../Shared/Vector.hpp"
#include <ncurses.h>

class Map {
public:
  Map();
  ~Map();

  void SetMapSize(const Vec2& size){
    if(m_pMap != nullptr){
      DeallocateMap(m_MapSize);
    }
    AllocateMap(size);
    m_MapSize = size;
  }
  
  void Update();

  [[nodiscard]]  uint8_t** GetMap(){
    return m_pMap;
  }

  static void RenderEntity(Map& map,Entity& entity){
    auto& pos = entity.GetPosition();
    if(IsOnMap(pos)){
      move(pos.y,pos.x);
      addch(entity.GetSymbol());
      map.GetMap()[pos.y][pos.x] = entity.GetSymbol();
    }
  }

  void Render();

private:
  void DeallocateMap(const Vec2& size){
    for (int i = 0; i < size.y; i++) {
      delete[] m_pMap[i];
    }
    delete[] m_pMap;
  }
  
  void AllocateMap(const Vec2& size){
    if(size.x == 0 || size.y == 0) return;
    
    m_pMap = new uint8_t *[size.y];
    for (int i = 0; i < size.y; i++) {
      m_pMap[i] = new uint8_t[size.x];
    }
  }

private:
  void CleanMap();
  void FillMap();
  
  static bool IsOnMap(const Vec2& pos){
    return !(pos.x <= 0 || pos.y <= 0 || pos.x >= (m_MapSize.x - 1) || pos.y >= (m_MapSize.y - 1));
  }
  
private:
  static Vec2 m_MapSize;
  uint8_t **m_pMap;
};

#endif //! SnakeGame_MAP_HPP