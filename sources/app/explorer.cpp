#include "explorer.hpp"

#include <imgui/imgui.h>

Explorer::Explorer() {}

void Explorer::handle_input( SDL_Event event )
{
    ( void )event;
}

void Explorer::render()
{
    ImGuiWindowFlags fullScreenflags = ImGuiWindowFlags_NoDecoration
                                       | ImGuiWindowFlags_NoMove
                                       | ImGuiWindowFlags_NoBackground;
    ImGui::SetNextWindowPos( ImGui::GetMainViewport()->Pos );
    ImGui::SetNextWindowSize( ImGui::GetMainViewport()->Size );

    static bool showDemoWindow = false;

    if ( ImGui::Begin( "Hello, world!", nullptr, fullScreenflags ) )
    {
        ImGui::Checkbox( "Show Demo Window", &showDemoWindow );
        ImGui::Text( "This is some text!" );
    }

    if ( showDemoWindow )
    {
        ImGui::ShowDemoWindow( &showDemoWindow );
    }
    ImGui::End();
}
