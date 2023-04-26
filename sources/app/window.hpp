#pragma once

#include <functional>
#include <memory>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
//

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

class Window
{
  public:
    enum class EventMode
    {
        Poll,
        Wait
    };

  private:
    GLFWwindow * m_window;
    EventMode    m_eventMode;
    bool         m_vsync;

  public:
    Window();
    virtual ~Window();

    Window( Window const & )              = delete;
    Window & operator= ( Window const & ) = delete;

    void update ( std::function< void() > callback );

    GLFWwindow *       get_backend ();
    GLFWwindow const * get_backend () const;

    ImVec2               get_size () const;
    ImVec2               get_display_scale () const;
    static ImVec2        get_display_size ();
    static GLFWmonitor * get_primary_monitor ();

    EventMode get_event_mode () const;
    void      set_event_mode ( EventMode eventMode );
    void      set_vsync ( bool vsync );
    bool      get_vsync () const;

    void reset_imgui_style () const;

  private:
    void initialize_GLFW ();
    void initialize_OpenGL () const;
    void initialize_ImGui () const;
    void terminate_SDL () const;
    void terminate_ImGui () const;

    static void new_frame ();
    static void clear ();
    static void render ();
};
