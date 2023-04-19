#include "application.hpp"

#include <glad/glad.h>
//
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "app/display.hpp"

Application::Application() : m_window {}, m_shouldRun { true }, m_explorer {} {}

void Application::handle_event()
{
    // SDL_Event event;
    // while ( SDL_PollEvent( &event ) )
    // {
    //     ImGui_ImplSDL2_ProcessEvent( &event );

    // switch ( event.type )
    // {
    // case SDL_QUIT :
    //     this->should_run( false );
    //     break;
    // case SDL_WINDOWEVENT_RESIZED :
    //     SDL_SetWindowSize( this->window().get_window_backend(),
    //                        event.window.data1, event.window.data2 );
    //     // glViewport( 0, 0, event.window.data1, event.window.data2 );
    //     // this->render();
    //     break;
    // }

    // m_explorer.handle_input( event );
    // }
}

void Application::render()
{
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    m_explorer.render();

    if ( ImGui::Begin( "Window", nullptr ) )
    {
        ImGuiIO & io = ImGui::GetIO();
        ImGui::Text( "Window size (ImGui): %.1f x %.1f", io.DisplaySize.x,
                     io.DisplaySize.y );
        ImGui::Text( "Window size (SDL): %.1f x %.1f",
                     this->window().get_size().x, this->window().get_size().y );
        ImGui::Text( "Display size : %.1f x %.1f", Window::get_display_size().x,
                     Window::get_display_size().y );
        ImGui::Text( "Display scaling: %.2f (X) x %.2f (Y)",
                     io.DisplayFramebufferScale.x,
                     io.DisplayFramebufferScale.y );
        ImGui::Text( "FPS: %.1f", io.Framerate );

        {
            float xscale, yscale;
            glfwGetMonitorContentScale( this->window().get_primary_monitor(),
                                        &xscale, &yscale );
            ImGui::Text( "Monitor scaling: %.2f (X) x %.2f (Y)", xscale,
                         yscale );
        }
        {
            float xscale, yscale;
            glfwGetWindowContentScale( this->window().get_window_backend(),
                                       &xscale, &yscale );
            ImGui::Text( "Window scaling: %.2f (X) x %.2f (Y)", xscale,
                         yscale );
        }

        ImGui::Text( "Font Name: %s", display::get_font_name().c_str() );
        ImGui::Text( "Font Size: %u", display::get_font_size() );
        ImGui::Text( "Text Scaling: %.2f", display::get_text_scaling_factor() );
    }
    ImGui::End();

    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize( m_window.get_window_backend(), &display_w,
                            &display_h );
    glViewport( 0, 0, display_w, display_h );
    glClearColor( 1.f, 1.f, 1.f, 1.00f );

    glClear( GL_COLOR_BUFFER_BIT );
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

    glfwSwapBuffers( m_window.get_window_backend() );
}

void Application::should_run( bool should_run )
{
    m_shouldRun = should_run;
}

bool Application::should_run()
{
    return m_shouldRun
           && glfwWindowShouldClose( m_window.get_window_backend() ) == 0;
}

Window const & Application::window() const
{
    return m_window;
}

Window & Application::window()
{
    return m_window;
}
