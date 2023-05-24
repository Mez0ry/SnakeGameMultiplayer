#ifndef __RANDOM_HPP__
#define  __RANDOM_HPP__
#include <random>
#include "../Shared/Vector.hpp"

namespace Random{

    extern auto GetRandomIntInRange(int a,int b) -> int{
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> distr(a,b);
        return distr(rng);
    }
    
    extern auto GetRandomPositionInRange(const Vec2& lhs_range, const Vec2& rhs_range) -> Vec2{
        return {GetRandomIntInRange(lhs_range.x,lhs_range.y),GetRandomIntInRange(rhs_range.x,rhs_range.y)};
    }
    
    extern auto GetRandomPositionInRange(const Vec2& range) -> Vec2{
        return {GetRandomIntInRange(range.x,range.y),GetRandomIntInRange(range.x,range.y)};
    }

};//!namespace
#endif //!__RANDOM_HPP__