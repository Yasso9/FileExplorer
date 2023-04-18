#pragma once

#include <memory>

#include <SDL/SDL.h>
#include <imgui/imgui.h>

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

    ImVec2        get_size ();
    static ImVec2 get_display_size ( int displayIndex = 0 );

  private:
    void initialize_SDL ();
    void initialize_OpenGL ();
    void initialize_ImGui ();
    void terminate_SDL ();
    void terminate_ImGui ();
};
