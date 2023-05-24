#ifndef __FOOD_HPP__
#define __FOOD_HPP__
#include "Entity.hpp"
#include "../Shared/Vector.hpp"

class Food : public Entity {
private:
  uint8_t m_FoodSymbol = '+';

public:
  Food();
  ~Food();

  void SetPosition(const Vec2 &pos) override { m_Position = pos; }

  const Vec2 &GetPosition() const override { return m_Position; };

  uint8_t GetSymbol() const override { return m_FoodSymbol; }

private:
  Vec2 m_Position;
};
#endif //!__FOOD_HPP__