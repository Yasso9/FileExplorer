#include "window.hpp"

#include <filesystem>
#include <iostream>

#include <fmt/core.h>
#include <fmt/format.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "app/display.hpp"
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

GLFWwindow * Window::get_backend()
{
    return m_window;
}

ImVec2 Window::get_size() const
{
    int width, height;
    glfwGetWindowSize( m_window, &width, &height );
    return ImVec2 { static_cast< float >( width ),
                    static_cast< float >( height ) };
}

ImVec2 Window::get_display_scale() const
{
    float xscale, yscale;
    glfwGetWindowContentScale( m_window, &xscale, &yscale );
    return ImVec2 { xscale, yscale };
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

GLFWmonitor * Window::get_primary_monitor()
{
    GLFWmonitor * primaryMonitor = glfwGetPrimaryMonitor();
    if ( ! primaryMonitor )
    {
        Trace::Error( "GLFW can't get primary monitor" );
    }

    return primaryMonitor;
}

void Window::new_frame() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::clear() const
{
    glViewport( 0, 0, this->get_size().x, this->get_size().y );
    glClearColor( 1.f, 1.f, 1.f, 1.f );
    glClear( GL_COLOR_BUFFER_BIT );
}

void Window::render() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}

void Window::swap_buffers() const
{
    glfwSwapBuffers( m_window );
}

namespace
{
    void error_callback ( int error, const char * description )
    {
        Trace::Error( fmt::format( "GLFW {}: {}\n", error, description ) );
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

    m_window =
        glfwCreateWindow( 1280, 720, "File Explorer (DEV)", nullptr, nullptr );
    if ( m_window == nullptr )
    {
        Trace::Error( "GLFW can't create window" );
    }

    glfwMakeContextCurrent( m_window );
    glfwSwapInterval( 1 );  // Enable vsync
}

void Window::initialize_OpenGL() const
{
    if ( ! gladLoadGLLoader(
             reinterpret_cast< GLADloadproc >( glfwGetProcAddress ) ) )
    {
        std::cerr << "[ERROR] Couldn't initialize glad" << std::endl;
    }

    Trace::Info( fmt::format(
        "OpenGL renderer: {}",
        reinterpret_cast< const char * >( glGetString( GL_VERSION ) ) ) );
    Trace::Info( fmt::format( "OpenGL version: {}.{}", GLVersion.major,
                              GLVersion.minor ) );
}

void Window::reset_imgui_style() const
{
    float fontScale =
        display::get_font_size() * display::get_text_scaling_factor() * 3.5f;
    float uiScale =
        std::sqrt( this->get_display_scale().x * this->get_display_scale().y );

    Trace::Info( fmt::format( "Font scale: {}", fontScale ) );
    Trace::Info( fmt::format( "UI scale: {}", uiScale ) );

    ImGuiIO & io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF( "../resources/fonts/Inter-Regular.ttf",
                                  fontScale );

    ImGui::StyleColorsDark();

    ImGuiStyle & style = ImGui::GetStyle();

    style.FrameRounding     = 6.f;
    style.GrabRounding      = 4.f;
    style.WindowRounding    = 10.f;
    style.ScrollbarRounding = 12.f;
    style.PopupRounding     = 6.f;

    style.WindowBorderSize = 0.f;
    style.ChildBorderSize  = 0.f;
    style.FrameBorderSize  = 0.f;
    style.PopupBorderSize  = 0.f;
    style.TabBorderSize    = 0.f;

    style.FramePadding  = ImVec2 { 10.f, 7.f };
    style.ItemSpacing   = ImVec2 { 10.f, 8.f };
    style.IndentSpacing = 15.f;
    style.ScrollbarSize = 18.f;
    style.GrabMinSize   = 18.f;

    style.CellPadding      = ImVec2 { 10.f, 10.f };
    style.ItemSpacing      = ImVec2 { 10.f, 8.f };
    style.ItemInnerSpacing = ImVec2 { 10.f, 8.f };

    style.SeparatorTextPadding = ImVec2 { 0.f, 20.f };

    style.WindowTitleAlign = ImVec2 { 0.5f, 0.5f };

    style.ScaleAllSizes( uiScale );
}

void Window::initialize_ImGui() const
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    this->reset_imgui_style();

    ImGui_ImplGlfw_InitForOpenGL( m_window, true );
    ImGui_ImplOpenGL3_Init( "#version 460" );
}

void Window::terminate_SDL() const
{
    glfwDestroyWindow( m_window );
    glfwTerminate();
}

void Window::terminate_ImGui() const
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
