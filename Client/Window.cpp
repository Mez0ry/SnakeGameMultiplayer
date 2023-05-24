#include "Window.hpp"

Window::Window() : m_Window(nullptr),m_WindowWidth(0), m_WindowHeight(0) {
  UpdateResolution();
  m_Window = initscr(); // initialise the screen
  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support color\n");
  }
}

Window::~Window() { endwin(); }

int Window::GetWindowWidth() const { return m_WindowWidth; }

int Window::GetWindowHeight() const { return m_WindowHeight; }