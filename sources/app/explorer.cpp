#include "explorer.hpp"

#include <imgui/imgui.h>

Explorer::Explorer() {}

void Explorer::handle_input( SDL_Event event )
{
    ( void )event;
}

void Explorer::render()
{
    bool showDemoWindow = true;
    if ( showDemoWindow )
    {
        ImGui::ShowDemoWindow( &showDemoWindow );
    }

    if ( ImGui::Begin( "Hello, world!" ) )
    {
        ImGui::Text( "This is some text!" );
    }
    ImGui::End();
}
