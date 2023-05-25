#ifndef __STATS_HPP__
#define __STATS_HPP__
#include <cstdint>
#include <string>

typedef struct Stats{
    Stats() : Wins(0),Losses(0){}
    ~Stats() = default;
    uint32_t Wins;
    uint32_t Losses;
};

#endif //!__STATS_HPP__