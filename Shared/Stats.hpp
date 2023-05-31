#ifndef __STATS_HPP__
#define __STATS_HPP__
#include <cstdint>
#include <string>

typedef struct Stats{
    Stats() : wins(0),losses(0){}
    ~Stats() = default;
    uint32_t wins;
    uint32_t losses;
};

#endif //!__STATS_HPP__