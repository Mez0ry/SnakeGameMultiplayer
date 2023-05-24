#ifndef __CLIENT_DATA_HPP__
#define __CLIENT_DATA_HPP__
#include <cstdint>
#include <string>

extern "C"{

  typedef struct ClientData {
    uint32_t id;
    std::string sUsername;
  };
  
}

#endif //! __CLIENT_DATA_HPP__