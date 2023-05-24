#ifndef SnakeGame_SNAKE_HPP
#define SnakeGame_SNAKE_HPP
#include "../Shared/Vector.hpp"
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <enet/enet.h>
#include "Entity.hpp"

class Snake : public Entity {
private:
  uint8_t m_SnakeSymbol = '0';

public:
  Snake();
  ~Snake();

  void SetDirection(const Vec2& direction){
    m_Direction = direction;
  }

  void SetPosition(const Vec2& pos) override{
    m_Position = pos;
  }

  const Vec2& GetPosition() const override {return m_Position;};

  uint8_t GetSymbol() const override {return m_SnakeSymbol;}
private:
  Vec2 m_Position;
  Vec2 m_Direction;
};

#endif //! SnakeGame_SNAKE_HPP