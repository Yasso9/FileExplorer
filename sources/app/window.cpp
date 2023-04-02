#include "window.hpp"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl2.h>

#include "tools/traces.hpp"

using namespace std::string_literals;  // Enables "s" suffix for std::string

Window::Window() : m_window { nullptr }, m_glContext { nullptr }
{
    this->initialize_SDL();
    this->initialize_ImGui();
}

Window::~Window()
{
    this->terminate_ImGui();
    this->terminate_SDL();
}

SDL_Window * Window::getSDLWindow()
{
    return m_window;
}

void Window::initialize_SDL()
{
    if ( SDL_Init( SDL_INIT_VIDEO ) != 0 )
    {
        Trace::Error( "SDL_Init Error - "s + SDL_GetError() );
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,
                         SDL_GL_CONTEXT_PROFILE_CORE );

    m_window = SDL_CreateWindow( "Explorer (DEV)", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, 1200, 720,
                                 SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
                                     | SDL_WINDOW_ALLOW_HIGHDPI
                                     | SDL_WINDOW_RESIZABLE );

    if ( ! m_window )
    {
        Trace::Error( "SDL_CreateWindow Error - "s + SDL_GetError() );
    }

    m_glContext = SDL_GL_CreateContext( this->getSDLWindow() );
    if ( ! m_glContext )
    {
        Trace::Error( "SDL_GL_CreateContext Error - "s + SDL_GetError() );
    }
}

void Window::initialize_ImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO & io = ImGui::GetIO();
    ( void )io;
    ImGui_ImplSDL2_InitForOpenGL( this->getSDLWindow(), m_glContext );
    ImGui_ImplOpenGL3_Init( "#version 330" );
}

void Window::terminate_SDL()
{
    SDL_GL_DeleteContext( m_glContext );
    SDL_DestroyWindow( this->getSDLWindow() );
    SDL_Quit();
}

void Window::terminate_ImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}
