#ifndef __GAME_SCORE_HPP__
#define __GAME_SCORE_HPP__
#include <cstdint>
#include <vector>
#include <memory>

namespace GameScore{

    typedef struct GameScore{
        uint32_t client_id;
        int score;
    };
    
    static std::vector<GameScore> g_GameScoreVec;

};

#endif //!__GAME_SCORE_HPP__