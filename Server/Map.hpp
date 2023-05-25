#ifndef __MAP_HPP__
#define __MAP_HPP__
#include "../Shared/Vector.hpp"

namespace Map{
    extern const Vec2 g_MapSize ={20,20};

    extern auto IsOnMap(const Vec2& pos) -> bool{
        return !(pos.x <= 0 || pos.y <= 0 || pos.x >= (g_MapSize.x - 1) || pos.y >= (g_MapSize.y - 1));
    }

};//! namespace

#endif //! __MAP_HPP__