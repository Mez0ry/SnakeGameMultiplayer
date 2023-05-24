#include "Map.hpp"
Vec2 Map::m_MapSize = {0,0};

Map::Map() : m_pMap(nullptr){
  CleanMap();
}

Map::~Map() {
}

void Map::Update() {
  CleanMap();
  FillMap();
}

void Map::Render() {
  for (int i = 0; i < m_MapSize.y; i++) {
    for (int j = 0; j < m_MapSize.x; j++) {
      move(i,j);
      addch(m_pMap[i][j] | A_BOLD);
    }
  }
}

void Map::CleanMap() {

  for (int i = 0; i < m_MapSize.y; i++) {
    for (int j = 0; j < m_MapSize.x; j++) {
      m_pMap[i][j] = ' ';
    }
  }
}

void Map::FillMap() {
  for (int i = 0; i < m_MapSize.y; i++) {
    for (int j = 0; j < m_MapSize.x; j++) {
      if (i == 0 || i == m_MapSize.y - 1) {
        m_pMap[i][j] = '*';
      } else if (i > 0 && i < m_MapSize.y - 1) {
        m_pMap[i][0] = '*';
        m_pMap[i][m_MapSize.x - 1] = '*';
      }

    }
  }
}
