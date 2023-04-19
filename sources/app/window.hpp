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

    GLFWwindow * get_window_backend ();

    ImVec2               get_size ();
    static ImVec2        get_display_size ();
    static GLFWmonitor * get_primary_monitor ();

  private:
    void initialize_GLFW ();
    void initialize_OpenGL ();
    void initialize_ImGui ();
    void terminate_SDL ();
    void terminate_ImGui ();
};
