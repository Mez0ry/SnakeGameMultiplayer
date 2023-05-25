#ifndef __CLIENT_DATA_HPP__
#define __CLIENT_DATA_HPP__
#include <cstdint>
#include <string>
#include "Stats.hpp"

typedef struct ClientData {
  ClientData() : score(0),id(0){}
  uint32_t id;
  std::string sUsername;
  Stats stats;
  uint32_t score;
};

#endif //! __CLIENT_DATA_HPP__