#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include "ncurses.h"

class Window {
public:
  Window();
  ~Window();

  int GetWindowWidth() const;

  int GetWindowHeight() const;
  
  [[nodiscard]] WINDOW *GetWindow() { 
    return m_Window; 
  }

  void UpdateResolution() {
    getmaxyx(stdscr, (m_WindowHeight), (m_WindowWidth));
  }

private:
  WINDOW *m_Window;
  int m_WindowWidth, m_WindowHeight;
};
#endif //!__WINDOW_HPP__