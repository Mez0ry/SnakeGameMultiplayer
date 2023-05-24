#include "Map.hpp"
Map::Map() {
  m_pMap = new uint8_t *[MAP_ROWS];
  for (int i = 0; i < MAP_ROWS; i++) {
    m_pMap[i] = new uint8_t[MAP_COLS];
  }
  CleanMap();
}

Map::~Map() {
  for (int i = 0; i < MAP_ROWS; i++) {
    delete[] m_pMap[i];
  }
  delete[] m_pMap;
}

void Map::Update() {
  CleanMap();
  FillMap();
}

void Map::Render() {
  for (int i = 0; i < MAP_ROWS; i++) {
    for (int j = 0; j < MAP_COLS; j++) {
      move(i,j);
      addch(m_pMap[i][j] | A_BOLD);
    }
  }
}

void Map::CleanMap() {

  for (int i = 0; i < MAP_ROWS; i++) {
    for (int j = 0; j < MAP_COLS; j++) {
      m_pMap[i][j] = ' ';
    }
  }
}

void Map::FillMap() {
  for (int i = 0; i < MAP_ROWS; i++) {
    for (int j = 0; j < MAP_COLS; j++) {
      if (i == 0 || i == MAP_ROWS - 1) {
        m_pMap[i][j] = '*';
      } else if (i > 0 && i < MAP_ROWS - 1) {
        m_pMap[i][0] = '*';
        m_pMap[i][MAP_COLS - 1] = '*';
      }

    }
  }
}
