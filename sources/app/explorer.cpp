#include "explorer.hpp"

#include <iostream>

#include <imgui/imgui.h>

#include "app/filesystem.hpp"

Explorer::Explorer() {}

void Explorer::handle_input( SDL_Event event )
{
    ( void )event;
}

void Explorer::render()
{
    static bool showDemoWindow = false;

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
        fs::path baseDirectory { ds::get_home_directory() };

        ds::show_folder_gui( baseDirectory );
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleVar( 2 );

    if ( showDemoWindow )
    {
        ImGui::ShowDemoWindow( &showDemoWindow );
    }
    ImGui::End();
}
