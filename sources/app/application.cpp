#include "application.hpp"

#include <glad/glad.h>
//
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "app/display.hpp"

Application::Application()
  : m_window {}, m_shouldRun { true }, m_explorer { m_window }
{}

void Application::update()
{
    if ( glfwWindowShouldClose( m_window.get_backend() ) )
    {
        m_shouldRun = false;
        return;
    }

    m_window.new_frame();

    m_window.clear();

    this->update_elements();

    m_window.render();
    m_window.swap_buffers();

    // glfwPollEvents();
    glfwWaitEvents();
}

void Application::update_elements()
{
    m_explorer.update();
}

void Application::should_run( bool should_run )
{
    m_shouldRun = should_run;
}

bool Application::should_run() const
{
    return m_shouldRun;
}
