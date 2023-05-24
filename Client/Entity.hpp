#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__
#include <cstdint>

struct Vec2;

class Entity{
public:
    virtual ~Entity() {}
    virtual const Vec2& GetPosition() const = 0;

    virtual void SetPosition(const Vec2& pos) = 0;

    virtual uint8_t GetSymbol() const = 0;

};
#endif //!__ENTITY_HPP__