#include "filesystem.hpp"

#include <iostream>

#include <fmt/core.h>
#include <imgui/imgui.h>

namespace ds
{
    fs::path get_home_directory ()
    {
        return fs::path( std::getenv( "HOME" ) );
    }

    bool is_showed_gui ( fs::directory_entry entry )
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

    uintmax_t get_folder_size ( fs::path folder )
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

    fs::path show_folder_gui ( fs::path directory )
    {
        fs::path choosenDirectory { directory };

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

            int row { 0 };
            for ( auto const & entry : fs::directory_iterator( directory ) )
            {
                if ( ! ds::is_showed_gui( entry )
                     || ( ! showHidden && is_hidden( entry ) ) )
                {
                    continue;
                }

                ImGui::TableNextRow( ImGuiTableRowFlags_None, 42.f );
                for ( int column = 0; column < nbColumns; column++ )
                {
                    ImGui::TableSetColumnIndex( column );
                    std::string label { "None" };
                    switch ( column )
                    {
                    case 0 :
                        label = entry.path().filename().string();
                        break;
                    case 1 :
                        label = std::to_string( get_size( entry ) );
                        break;
                    case 2 :
                        label = ds::get_type( entry ).c_str();
                        break;
                    default :
                        label = "N/A";
                        break;
                    }
                    ImGuiSelectableFlags selectable_flags =
                        ImGuiSelectableFlags_SpanAllColumns;
                    // | ImGuiSelectableFlags_AllowItemOverlap;
                    bool        isSelected { false };
                    std::string id {
                        fmt::format( "{}##{},{}", label, column, row ) };
                    ImGui::Selectable( id.c_str(), &isSelected,
                                       selectable_flags, ImVec2( 0, 30 ) );

                    if ( ImGui::IsItemHovered()
                         && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left )
                         && entry.is_directory() )
                    {
                        choosenDirectory = entry.path();
                        std::cout << "Double Clicked: "
                                  << entry.path().filename().string()
                                  << std::endl;
                    }
                }
                ++row;
            }
        }
        ImGui::EndTable();

        return choosenDirectory;
    }
}  // namespace ds
