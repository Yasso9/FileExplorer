#include "explorer.hpp"

#include <iostream>

#include <fmt/core.h>

#include "app/display.hpp"

Explorer::Explorer()
  : m_showDemoWindow { false },
    m_showInfoWindow { false },
    m_showWindowConfig { false },
    m_showHidden { false },
    m_currentDirectory { ds::get_home_directory() },
    m_backgroundColor { 0.2f, 0.2f, 0.2f, 1.f }
{}

namespace
{
    void information ( Window const & window )
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

    void window_configuration ( Window & window )
    {
        if ( ImGui::Begin( "Window Configuration", nullptr ) )
        {
            ImGui::Text( "Event Mode" );
            if ( ImGui::RadioButton( "Poll", window.get_event_mode()
                                                 == Window::EventMode::Poll ) )
            {
                window.set_event_mode( Window::EventMode::Poll );
            }
            ImGui::SameLine();
            if ( ImGui::RadioButton( "Wait", window.get_event_mode()
                                                 == Window::EventMode::Wait ) )
            {
                window.set_event_mode( Window::EventMode::Wait );
            }

            bool vsync = window.get_vsync();
            if ( ImGui::Checkbox( "Vsync", &vsync ) )
            {
                window.set_vsync( vsync );
            }
        }
        ImGui::End();
    }
}  // namespace

void Explorer::update( Window & window )
{
    ImGuiWindowFlags fullScreenflags = ImGuiWindowFlags_NoDecoration
                                       | ImGuiWindowFlags_NoMove
                                       | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::SetNextWindowPos( ImGui::GetMainViewport()->Pos );
    ImGui::SetNextWindowSize( ImGui::GetMainViewport()->Size );
    ImGui::PushStyleColor( ImGuiCol_WindowBg, m_backgroundColor );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
    if ( ImGui::Begin( "File Explorer", nullptr, fullScreenflags ) )
    {
        ImGui::Checkbox( "Show Demo Window", &m_showDemoWindow );
        ImGui::Checkbox( "Show Informations", &m_showInfoWindow );
        ImGui::Checkbox( "Show Window Configuration", &m_showWindowConfig );
        ImGui::Checkbox( "Show Hidden Files/Folder", &m_showHidden );

        // Button to go to the parent directory and button to go to the home
        // directory
        if ( ImGui::Button( "Parent" ) )
        {
            m_currentDirectory = m_currentDirectory.parent_path();
        }
        ImGui::SameLine();
        if ( ImGui::Button( "Home" ) )
        {
            m_currentDirectory = ds::get_home_directory();
        }
        ImGui::SameLine();
        ImGui::InputText(
            "##Current Directory",
            const_cast< char * >( m_currentDirectory.string().c_str() ),
            m_currentDirectory.string().size() + 1 );

        // Button to go to the previous directory and button to go to the
        // next directory if ( ImGui::Button( "Previous" ) )
        // {
        //     m_currentDirectory = this->get_previous_directory();
        // }
        // ImGui::SameLine();
        // if ( ImGui::Button( "Next" ) )
        // {
        //     m_currentDirectory = this->get_next_directory();
        // }

        this->update_table_gui();
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar( 2 );

    if ( m_showDemoWindow )
    {
        ImGui::ShowDemoWindow( &m_showDemoWindow );
    }
    if ( m_showInfoWindow )
    {
        information( window );
    }
    if ( m_showWindowConfig )
    {
        window_configuration( window );
    }
}

void Explorer::update_table_gui()
{
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable
                            | ImGuiTableFlags_NoBordersInBodyUntilResize;
    int nbColumns { 3 };

    if ( ImGui::BeginTable( "Filesystem Item List", nbColumns, flags ) )
    {
        ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthStretch );
        ImGui::TableSetupColumn( "Size", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableHeadersRow();

        int row { 0 };
        for ( auto const & entry :
              fs::directory_iterator( m_currentDirectory ) )
        {
            if ( ! ds::is_showed_gui( entry )
                 || ( ! m_showHidden && ds::is_hidden( entry ) ) )
            {
                continue;
            }

            this->update_row_gui( entry, nbColumns, row );
            ++row;
        }
    }
    ImGui::EndTable();
}

void Explorer::update_row_gui( fs::directory_entry entry, int nbColumns,
                               int row )
{
    ImGui::TableNextRow( ImGuiTableRowFlags_None );
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
            label = std::to_string( ds::get_size( entry ) );
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
        std::string id { fmt::format( "{}##{},{}", label, column, row ) };
        ImGui::Selectable( id.c_str(), &isSelected, selectable_flags );

        if ( ImGui::IsItemHovered()
             && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left )
             && entry.is_directory() )
        {
            m_currentDirectory = entry.path();
            std::cout << "Double Clicked: " << entry.path().filename().string()
                      << std::endl;
        }
    }
}
