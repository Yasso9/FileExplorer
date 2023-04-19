#include "explorer.hpp"

#include <iostream>

#include <imgui/imgui.h>

#include "app/display.hpp"
#include "app/filesystem.hpp"

Explorer::Explorer( Window const & window ) : m_window { window } {}

namespace
{
    void info_window ( Window const & window )
    {
        if ( ImGui::Begin( "Window", nullptr ) )
        {
            ImGuiIO & io = ImGui::GetIO();
            ImGui::Text( "Window size (ImGui): %.1f x %.1f", io.DisplaySize.x,
                         io.DisplaySize.y );
            ImGui::Text( "Window size (GLFW): %.1f x %.1f", window.get_size().x,
                         window.get_size().y );
            ImGui::Text( "Display size : %.1f x %.1f",
                         Window::get_display_size().x,
                         Window::get_display_size().y );
            ImGui::Text( "Display scaling (ImGui): %.1f x %.1f",
                         io.DisplayFramebufferScale.x,
                         io.DisplayFramebufferScale.y );
            ImGui::Text( "Display scaling (GLFW): %.1f x %.1f",
                         window.get_display_scale().x,
                         window.get_display_scale().y );

            ImGui::Separator();

            ImGui::Text( "FPS: %.1f", io.Framerate );

            ImGui::Separator();

            ImGui::Text( "Font Name: %s", display::get_font_name().c_str() );
            ImGui::Text( "Font Size: %u", display::get_font_size() );
            ImGui::Text( "Text Scaling: %.2f",
                         display::get_text_scaling_factor() );
        }
        ImGui::End();
    }
}  // namespace

void Explorer::update()
{
    static bool     showDemoWindow = false;
    static bool     showInfoWindow = false;
    static fs::path currentDir { ds::get_home_directory() };

    ImGuiWindowFlags fullScreenflags = ImGuiWindowFlags_NoDecoration
                                       | ImGuiWindowFlags_NoMove
                                       | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::SetNextWindowPos( ImGui::GetMainViewport()->Pos );
    ImGui::SetNextWindowSize( ImGui::GetMainViewport()->Size );
    ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0.2f, 0.2f, 0.2f, 1.f ) );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
    if ( ImGui::Begin( "File Explorer", nullptr, fullScreenflags ) )
    {
        ImGui::Checkbox( "Show Demo Window", &showDemoWindow );
        ImGui::Checkbox( "Show Informations", &showInfoWindow );

        currentDir = ds::show_folder_gui( currentDir );
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar( 2 );

    if ( showDemoWindow )
    {
        ImGui::ShowDemoWindow( &showDemoWindow );
    }
    if ( showDemoWindow )
    {
        info_window( m_window );
    }
}
