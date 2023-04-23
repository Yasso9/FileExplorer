#include "window.hpp"

#include <filesystem>
#include <iostream>

#include <fmt/core.h>
#include <fmt/format.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "app/display.hpp"
#include "app/filesystem.hpp"
#include "tools/traces.hpp"

Window::Window()
  : m_window { nullptr }, m_eventMode { EventMode::Poll }, m_vsync { true }
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

void Window::update( std::function< void() > callback )
{
    this->new_frame();
    this->clear();

    callback();

    this->render();
    // todo put this if define imguiviewport exist
    // if ( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
    // {
    //     GLFWwindow * backup_current_context = glfwGetCurrentContext();
    //     ImGui::UpdatePlatformWindows();
    //     ImGui::RenderPlatformWindowsDefault();
    //     glfwMakeContextCurrent( backup_current_context );
    // }
    glfwSwapBuffers( m_window );

    switch ( m_eventMode )
    {
    case EventMode::Poll :
        glfwPollEvents();
        break;
    case EventMode::Wait :
        glfwWaitEvents();
        break;
    }
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

Window::EventMode Window::get_event_mode() const
{
    return m_eventMode;
}

void Window::set_event_mode( Window::EventMode eventMode )
{
    m_eventMode = eventMode;
}

void Window::set_vsync( bool vsync )
{
    m_vsync = vsync;
    glfwSwapInterval( vsync ? 1 : 0 );
}

bool Window::get_vsync() const
{
    return m_vsync;
}

namespace
{
    void error_callback ( int error, const char * description )
    {
        Trace::Error( fmt::format( "GLFW {}: {}\n", error, description ) );
    }

    void window_size_callback ( GLFWwindow * /* window */, int width,
                                int height )
    {
        // Trace::Info( fmt::format( "Window resize: {}x{}", width, height )
        // ); todo use the window render function
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
        glViewport( 0, 0, width, height );
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

    m_window = glfwCreateWindow( Window::get_display_size().x * 0.7,
                                 Window::get_display_size().y * 0.7,
                                 "File Explorer (DEV)", nullptr, nullptr );
    glfwSetWindowSizeCallback( m_window, window_size_callback );

    if ( m_window == nullptr )
    {
        Trace::Error( "GLFW can't create window" );
    }

    glfwMakeContextCurrent( m_window );
    this->set_vsync( m_vsync );
}

void Window::initialize_OpenGL() const
{
    if ( ! gladLoadGLLoader(
             reinterpret_cast< GLADloadproc >( glfwGetProcAddress ) ) )
    {
        Trace::Error( "Couldn't initialize glad" );
    }

    glViewport( 0, 0, this->get_size().x, this->get_size().y );

    Trace::Info( fmt::format(
        "OpenGL renderer: {}",
        reinterpret_cast< const char * >( glGetString( GL_VERSION ) ) ) );
    Trace::Info( fmt::format( "OpenGL version: {}.{}", GLVersion.major,
                              GLVersion.minor ) );
}

namespace
{
    fs::path get_executable ()
    {
        std::string path;

        char    buffer[PATH_MAX];
        ssize_t length =
            readlink( "/proc/self/exe", buffer, sizeof( buffer ) - 1 );
        if ( length == -1 )
        {
            Trace::Error( "Can't get executable path" );
        }

        // readlink doesn't add the null terminator
        buffer[length] = '\0';
        return fs::path { buffer };
    }

    fs::path get_executable_dir ()
    {
        return get_executable().parent_path();
    }

    fs::path get_resources_dir ()
    {
        return get_executable_dir().parent_path() / "resources";
    }
}  // namespace

void Window::reset_imgui_style() const
{
    float uiScale =
        std::sqrt( this->get_display_scale().x * this->get_display_scale().y );

    Trace::Info( fmt::format( "UI scale: {}", uiScale ) );

    ImGuiIO & io = ImGui::GetIO();
    Trace::Info( fmt::format( "Current program path: {}",
                              fs::current_path().string() ) );
    Trace::Info(
        fmt::format( "Executable path: {}", get_executable_dir().string() ) );
    Trace::Info( fmt::format(
        "Resources path: {}",
        ( get_resources_dir() / "fonts/Inter-Regular.ttf" ).string() ) );
    io.Fonts->AddFontFromFileTTF(
        ( get_resources_dir() / "fonts/Inter-Regular.ttf" ).string().c_str(),
        uiScale * 20.f );

    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGuiStyle & style = ImGui::GetStyle();

    style.WindowPadding     = ImVec2 { 10.f, 10.f };
    style.FramePadding      = ImVec2 { 10.f, 10.f };
    style.CellPadding       = ImVec2 { 10.f, 10.f };
    style.ItemSpacing       = ImVec2 { 10.f, 10.f };
    style.ItemInnerSpacing  = ImVec2 { 10.f, 10.f };
    style.TouchExtraPadding = ImVec2 { 0.f, 0.f };

    style.IndentSpacing = 15.f;
    style.ScrollbarSize = 15.f;
    style.GrabMinSize   = 15.f;

    style.WindowBorderSize = 0.f;
    style.ChildBorderSize  = 0.f;
    style.FrameBorderSize  = 0.f;
    style.PopupBorderSize  = 0.f;
    style.TabBorderSize    = 0.f;

    style.WindowRounding    = 6.f;
    style.ChildRounding     = 6.f;
    style.FrameRounding     = 6.f;
    style.PopupRounding     = 6.f;
    style.ScrollbarRounding = 6.f;
    style.GrabRounding      = 6.f;
    style.TabRounding       = 6.f;

    style.WindowTitleAlign         = ImVec2 { 0.5f, 0.5f };
    style.WindowMenuButtonPosition = ImGuiDir_None;
    style.ColorButtonPosition      = ImGuiDir_Right;
    style.ButtonTextAlign          = ImVec2 { 0.5f, 0.5f };
    style.SelectableTextAlign      = ImVec2 { 0.f, 0.5f };
    style.SeparatorTextBorderSize  = 3.f;
    style.SeparatorTextAlign       = ImVec2 { 0.5f, 0.5f };
    style.SeparatorTextPadding     = ImVec2 { 0.5f, 0.5f };
    style.LogSliderDeadzone        = 0.5f;

    style.DisplaySafeAreaPadding = ImVec2 { 3.f, 3.f };

    style.Colors[ImGuiCol_TableHeaderBg] =
        ImVec4 { 0.25f, 0.25f, 0.25f, 1.00f };
    style.Colors[ImGuiCol_TableBorderLight] =
        ImVec4 { 0.80f, 0.80f, 0.80f, 1.00f };
    style.Colors[ImGuiCol_TableBorderStrong] =
        ImVec4 { 0.70f, 0.70f, 0.70f, 1.00f };
    style.Colors[ImGuiCol_Text]           = ImVec4 { 1.0f, 1.0f, 1.0f, 1.0f };
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4 { 0.0f, 0.0f, 0.0f, 1.0f };

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

void Window::new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::clear()
{
    glClearColor( 0.2f, 0.2f, 0.2f, 1.f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
             | GL_STENCIL_BUFFER_BIT );
}

void Window::render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}
