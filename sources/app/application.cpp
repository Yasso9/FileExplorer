#include "application.hpp"

#include <glad/glad.h>
//
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "app/display.hpp"

Application::Application() : m_window {}, m_shouldRun { true }, m_explorer {} {}

void Application::update()
{
    if ( glfwWindowShouldClose( m_window.get_backend() ) )
    {
        m_shouldRun = false;
        return;
    }

    m_window.update( [this] () { this->update_elements(); } );
}

void Application::update_elements()
{
    m_explorer.update( m_window );
}

void Application::should_run( bool should_run )
{
    m_shouldRun = should_run;
}

bool Application::should_run() const
{
    return m_shouldRun;
}
