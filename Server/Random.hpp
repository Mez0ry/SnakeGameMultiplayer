#ifndef __RANDOM_HPP__
#define  __RANDOM_HPP__
#include <random>
#include "../Shared/Vector.hpp"

namespace Random{

    auto get_random_int_in_range(int a,int b) -> int{
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> distr(a,b);
        return distr(rng);
    }
    
    auto get_random_vec2(const Vec2& lhs_range, const Vec2& rhs_range) -> Vec2{
        return {get_random_int_in_range(lhs_range.x,lhs_range.y),get_random_int_in_range(rhs_range.x,rhs_range.y)};
    }
    
    auto get_random_vec2(const Vec2& range) -> Vec2{
        return {get_random_int_in_range(range.x,range.y),get_random_int_in_range(range.x,range.y)};
    }

};//!namespace
#endif //!__RANDOM_HPP__