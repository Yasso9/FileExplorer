#include "window.hpp"

#include <filesystem>
#include <iostream>

#include <fmt/core.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "tools/traces.hpp"

Window::Window() : m_window { nullptr }
{
    this->initialize_GLFW();
    this->initialize_OpenGL();
    this->initialize_ImGui();
}

Window::~Window()
{
    this->terminate_ImGui();
    this->terminate_SDL();
}

GLFWwindow * Window::get_window_backend()
{
    return m_window;
}

ImVec2 Window::get_size()
{
    int width, height;
    glfwGetWindowSize( m_window, &width, &height );
    return ImVec2 { static_cast< float >( width ),
                    static_cast< float >( height ) };
}

GLFWmonitor * Window::get_primary_monitor()
{
    GLFWmonitor * primaryMonitor = glfwGetPrimaryMonitor();
    if ( ! primaryMonitor )
    {
        Trace::Error( "GLFW can't get primary monitor" );
    }

    return primaryMonitor;
}

ImVec2 Window::get_display_size()
{
    GLFWvidmode const * mode =
        glfwGetVideoMode( Window::get_primary_monitor() );
    if ( ! mode )
    {
        Trace::Error( "GLFW can't get video mode" );
    }

    return ImVec2 { static_cast< float >( mode->width ),
                    static_cast< float >( mode->height ) };
}

namespace
{
    void error_callback ( int error, const char * description )
    {
        Trace::Error( fmt::format( "GLFW {}: {}\n", error, description ) );
    }

    void window_content_scale_callback ( GLFWwindow * /* window */,
                                         float xscale, float yscale )
    {
        std::cout << "New Scale : " << xscale << "  " << yscale << std::endl;
    }

}  // namespace

void Window::initialize_GLFW()
{
    glfwSetErrorCallback( error_callback );
    if ( ! glfwInit() )
    {
        Trace::Error( "GLFW init" );
    }

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
    // glfwWindowHint( GLFW_SCALE_TO_MONITOR, GLFW_TRUE );

    // Create window with graphics context
    m_window =
        glfwCreateWindow( 1280, 720, "File Explorer (DEV)", nullptr, nullptr );
    if ( m_window == nullptr )
    {
        Trace::Error( "GLFW create window" );
    }
    glfwMakeContextCurrent( m_window );
    glfwSwapInterval( 1 );  // Enable vsync

    glfwSetWindowContentScaleCallback( m_window,
                                       window_content_scale_callback );
}

void Window::initialize_OpenGL()
{
    if ( ! gladLoadGLLoader(
             reinterpret_cast< GLADloadproc >( glfwGetProcAddress ) ) )
    {
        std::cerr << "[ERROR] Couldn't initialize glad" << std::endl;
    }

    std::cout << "[INFO] OpenGL renderer: " << glGetString( GL_RENDERER )
              << std::endl;
    std::cout << "[INFO] OpenGL from glad " << GLVersion.major << "."
              << GLVersion.minor << std::endl;

    // int height, width;
    // SDL_GetWindowSize( m_window, &width, &height );
    glViewport( 0, 0, this->get_size().x, this->get_size().y );
    glClearColor( 35 / 255.0f, 35 / 255.0f, 35 / 255.0f, 1.00f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
             | GL_STENCIL_BUFFER_BIT );
}

namespace
{
    void initialize_ImGui_style ( float uiScaleFactor )
    {
        ImGuiIO & io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF( "../resources/fonts/Inter-Regular.ttf",
                                      uiScaleFactor * 20.f );
        // io.DisplayFramebufferScale = ImVec2( 1.f, 1.f );
        // ImGuiIO & io = ImGui::GetIO();
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        // io.Fonts->AddFontFromFileTTF(
        // "./resources/fonts/Inter-Regular.ttf", 18.f
        // ); io.Fonts->AddFontFromFileTTF(
        // "./resources/fonts/Satoshi-Variable.ttf",
        //                               18.f );
        // io.Fonts->AddFontFromFileTTF(
        // "./resources/fonts/Switzer-Variable.ttf",
        //                               18.f );

        // ImGui::StyleColorsDark();
        ImGui::StyleColorsClassic();

        ImGuiStyle & style = ImGui::GetStyle();

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
        style.ScaleAllSizes( uiScaleFactor );
    }
}  // namespace

void Window::initialize_ImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    float uiScaleFactor = 1.f;
    initialize_ImGui_style( uiScaleFactor );

    ImGui_ImplGlfw_InitForOpenGL( m_window, true );
    ImGui_ImplOpenGL3_Init( "#version 460" );
}

void Window::terminate_SDL()
{
    glfwDestroyWindow( m_window );
    glfwTerminate();
}

void Window::terminate_ImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
