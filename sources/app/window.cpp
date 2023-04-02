#include "window.hpp"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl2.h>

#include "tools/traces.hpp"

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
    using namespace std::string_literals;  // Enables "s" suffix for std::string

    if ( SDL_Init( SDL_INIT_VIDEO ) != 0 )
    {
        Trace::Error( "SDL_Init Error - "s + SDL_GetError() );
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,
                         SDL_GL_CONTEXT_PROFILE_CORE );

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode( 0, &DM );
    auto displayWidth  = DM.w;
    auto displayHeight = DM.h;

    m_window = SDL_CreateWindow(
        "Explorer (DEV)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        displayWidth / 2, displayHeight / 2,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
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

namespace
{
    void initialize_ImGui_style ( float uiScaleFactor )
    {
        // ImGui::StyleColorsDark();
        ImGui::StyleColorsClassic();

        ImGuiStyle & style = ImGui::GetStyle();
        style.ScaleAllSizes( uiScaleFactor * 1.5f );

        style.FrameRounding     = 6.f;
        style.GrabRounding      = 4.f;
        style.WindowRounding    = 10.f;
        style.ScrollbarRounding = 12.f;
        style.PopupRounding     = 6.f;

        style.WindowBorderSize = 0.f;
        style.FrameBorderSize  = 0.f;
        style.PopupBorderSize  = 0.f;

        style.FramePadding  = ImVec2 { 10.f, 10.f };
        style.ItemSpacing   = ImVec2 { 10.f, 8.f };
        style.IndentSpacing = 15.f;
        style.ScrollbarSize = 18.f;
        style.GrabMinSize   = 18.f;

        style.WindowTitleAlign = ImVec2 { 0.5f, 0.5f };
    }
}  // namespace

void Window::initialize_ImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    float uiScaleFactor = 1.f;

    ImGuiIO & io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF( "./resources/fonts/Inter-Regular.ttf",
                                  uiScaleFactor * 20.f );
    // ImGuiIO & io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.Fonts->AddFontFromFileTTF( "./resources/fonts/Inter-Regular.ttf", 18.f
    // ); io.Fonts->AddFontFromFileTTF(
    // "./resources/fonts/Satoshi-Variable.ttf",
    //                               18.f );
    // io.Fonts->AddFontFromFileTTF( "./resources/fonts/Switzer-Variable.ttf",
    //                               18.f );

    initialize_ImGui_style( uiScaleFactor );

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
