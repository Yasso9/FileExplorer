#include "application.hpp"

#include <SDL/SDL.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl2.h>

Application::Application() : m_window {}, m_shouldRun { true }, m_explorer {} {}

void Application::handle_event()
{
    SDL_Event event;
    while ( SDL_PollEvent( &event ) )
    {
        ImGui_ImplSDL2_ProcessEvent( &event );

        switch ( event.type )
        {
        case SDL_QUIT :
            this->should_run( false );
            break;
        case SDL_WINDOWEVENT_RESIZED :
            glViewport( 0, 0, event.window.data1, event.window.data2 );
            break;
        }

        m_explorer.handle_input( event );
    }
}

void Application::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame( this->window().getSDLWindow() );
    ImGui::NewFrame();

    m_explorer.render();

    ImGui::Render();

    int height, width;
    SDL_GetWindowSize( this->window().getSDLWindow(), &width, &height );
    glViewport( 0, 0, width, height );
    glClearColor( 35 / 255.0f, 35 / 255.0f, 35 / 255.0f, 1.00f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
             | GL_STENCIL_BUFFER_BIT );

    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
    SDL_GL_SwapWindow( this->window().getSDLWindow() );
}

void Application::should_run( bool should_run )
{
    m_shouldRun = should_run;
}

bool Application::should_run() const
{
    return m_shouldRun;
}

Window const & Application::window() const
{
    return m_window;
}

Window & Application::window()
{
    return m_window;
}
