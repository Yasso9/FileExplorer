#include "explorer.hpp"

#include <filesystem>
#include <iostream>

#include <imgui/imgui.h>

namespace fs = std::filesystem;

namespace
{
    fs::path get_home_directory ()
    {
        return fs::path( std::getenv( "HOME" ) );
    }

    bool is_showed ( fs::directory_entry entry )
    {
        return entry.is_regular_file() || entry.is_directory()
               || entry.is_symlink();
    }

    bool is_hidden ( fs::directory_entry entry )
    {
        return entry.path().filename().string().find( '.' ) == 0;
    }

    std::string get_type ( fs::directory_entry entry )
    {
        std::string extension { entry.path().extension().string() };

        if ( extension == "txt" )
        {
            return "Text";
        }
        else if ( extension == "pdf" )
        {
            return "PDF";
        }
        else if ( extension == "jpg" || extension == "jpeg"
                  || extension == "png" )
        {
            return "Image";
        }
        else
        {
            return "Others";
        }
    }

    [[__maybe_unused__]] uintmax_t get_folder_size ( fs::path folder )
    {
        uintmax_t size = 0;

        for ( const auto & entry : fs::recursive_directory_iterator( folder ) )
        {
            if ( entry.is_regular_file() )
            {
                size += entry.file_size();
            }
        }

        return size;
    }

    uintmax_t get_size ( fs::directory_entry entry )
    {
        if ( entry.is_directory() )
        {
            // return get_folder_size( entry.path() );
            return 0;
        }
        else
        {
            return entry.file_size();
        }
    }

    void show_folder ( fs::path directory )
    {
        int             nbColumns = 3;
        ImGuiTableFlags flags     = ImGuiTableFlags_RowBg
                                | ImGuiTableFlags_Resizable
                                | ImGuiTableFlags_NoBordersInBodyUntilResize;

        static bool showHidden { false };
        ImGui::Checkbox( "Show Hidden", &showHidden );

        if ( ImGui::BeginTable( "File List", nbColumns, flags ) )
        {
            ImGui::TableSetupColumn( "Name",
                                     ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableSetupColumn( "Size", ImGuiTableColumnFlags_WidthFixed );
            ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed );
            ImGui::TableHeadersRow();

            for ( auto const & entry : fs::directory_iterator( directory ) )
            {
                if ( ! is_showed( entry )
                     || ( ! showHidden && is_hidden( entry ) ) )
                {
                    continue;
                }

                ImGui::TableNextRow( ImGuiTableRowFlags_None, 42.f );
                for ( int column = 0; column < nbColumns; column++ )
                {
                    ImGui::TableSetColumnIndex( column );
                    switch ( column )
                    {
                    case 0 :
                        ImGui::TextUnformatted(
                            entry.path().filename().string().c_str() );
                        break;
                    case 1 :
                        ImGui::TextUnformatted(
                            std::to_string( get_size( entry ) ).c_str() );
                        break;
                    case 2 :
                        ImGui::TextUnformatted( get_type( entry ).c_str() );
                        break;
                    default :
                        ImGui::TextUnformatted( "N/A" );
                        break;
                    }
                }
            }
            ImGui::EndTable();
        }
    }
}  // namespace

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
        fs::path baseDirectory { get_home_directory() };
        show_folder( baseDirectory );
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleVar( 2 );

    if ( showDemoWindow )
    {
        ImGui::ShowDemoWindow( &showDemoWindow );
    }
    ImGui::End();
}
