#ifndef SHARED_EVENTS_HPP
#define SHARED_EVENTS_HPP
#include <cstdint>

enum class Event : uint8_t{
  CLIENT_CONNECTED = 1,
  CLIENT_DISCONNECTED,
  BROADCAST_POSITION,
  DIRECTION,
  BROADCAST_SCORE ,
  USERNAME ,
  MAP_SIZE,
  BROADCAST_STATS,
  UNKNOWN = 255
};

enum class EntityType{
  SNAKE,
  FOOD
};

#endif //!SHARED_EVENTS_HPP