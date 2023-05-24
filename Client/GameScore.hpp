#ifndef __GAME_SCORE_HPP__
#define __GAME_SCORE_HPP__
#include <string>
#include <charconv>

class GameScore {
public:
  GameScore();
  ~GameScore();
  
  void SetScore(int score){
    m_Score = score;
  }
  
  int GetScore() const{
    return m_Score;
  }
  
  GameScore& operator+=(int val){
    m_Score += val;
    return (*this);
  }

  GameScore& operator-=(int val){
    m_Score -= val;
    return (*this);
  }
private:
  int m_Score;
};
#endif //!__GAME_SCORE_HPP__