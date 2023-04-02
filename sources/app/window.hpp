#pragma once

#include <memory>

#include <SDL/SDL.h>

class Window
{
    SDL_Window *  m_window;
    SDL_GLContext m_glContext;

  public:
    Window();
    virtual ~Window();

    Window( Window const & )              = delete;
    Window & operator= ( Window const & ) = delete;

    SDL_Window * getSDLWindow ();

  private:
    void initialize_SDL ();
    void initialize_ImGui ();
    void terminate_SDL ();
    void terminate_ImGui ();
};
