#ifndef SHARED_EVENTS_HPP
#define SHARED_EVENTS_HPP
#include <cstdint>

enum class Event : uint8_t{
  CLIENT_ID = 1,
  BROADCAST_POSITION = 2,
  DIRECTION = 3,
  BROADCAST_SCORE = 4,
  USERNAME = 5,
  UNKNOWN = 255
};

enum class EntityType{
  SNAKE,
  FOOD
};

#endif //!SHARED_EVENTS_HPP