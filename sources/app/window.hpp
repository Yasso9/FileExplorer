#pragma once

#include <memory>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
//
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

class Window
{
    GLFWwindow * m_window;

  public:
    Window();
    virtual ~Window();

    Window( Window const & )              = delete;
    Window & operator= ( Window const & ) = delete;

    GLFWwindow *       get_backend ();
    GLFWwindow const * get_backend () const;

    ImVec2               get_size () const;
    ImVec2               get_display_scale () const;
    static ImVec2        get_display_size ();
    static GLFWmonitor * get_primary_monitor ();

    void new_frame () const;
    void clear () const;
    void render () const;
    void swap_buffers () const;

    void reset_imgui_style () const;

  private:
    void initialize_GLFW ();
    void initialize_OpenGL () const;
    void initialize_ImGui () const;
    void terminate_SDL () const;
    void terminate_ImGui () const;
};
