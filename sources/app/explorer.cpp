#include "explorer.hpp"

#include <climits>
#include <iostream>
#include <optional>

#include <fmt/core.h>

#include "app/display.hpp"
#include "tools/string.hpp"
#include "tools/traces.hpp"

namespace
{
    void information ( Window const & window )
    {
        ImGuiIO & io = ImGui::GetIO();
        ImGui::Text( "Window size (ImGui): %.1f x %.1f", io.DisplaySize.x,
                     io.DisplaySize.y );
        ImGui::Text( "Window size (GLFW): %.1f x %.1f", window.get_size().x,
                     window.get_size().y );
        ImGui::Text( "Display size : %.1f x %.1f", Window::get_display_size().x,
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
        ImGui::Text( "Text Scaling: %.2f", display::get_text_scaling_factor() );
    }

    void window_configuration ( Window & window )
    {
        ImGui::Text( "Event Mode" );
        if ( ImGui::RadioButton(
                 "Poll", window.get_event_mode() == Window::EventMode::Poll ) )
        {
            window.set_event_mode( Window::EventMode::Poll );
        }
        ImGui::SameLine();
        if ( ImGui::RadioButton(
                 "Wait", window.get_event_mode() == Window::EventMode::Wait ) )
        {
            window.set_event_mode( Window::EventMode::Wait );
        }

        bool vsync = window.get_vsync();
        if ( ImGui::Checkbox( "Vsync", &vsync ) )
        {
            window.set_vsync( vsync );
        }
    }

    std::vector< fs::path > filter_entry ( fs::path const &    directory,
                                           std::string const & entryFilter,
                                           bool                showHidden )
    {
        std::vector< fs::path > entries {};

        // Check if the directory exists
        if ( ! fs::exists( directory ) && ! fs::is_directory( directory ) )
        {
            return entries;
        }
        for ( auto const & entry : fs::directory_iterator( directory ) )
        {
            if ( ! showHidden && ds::is_hidden( entry ) )
            {
                continue;
            }

            if ( string::to_lowercase( entry.path().filename().string() )
                     .find( string::to_lowercase( entryFilter ) )
                 != std::string::npos )
            {
                entries.push_back( entry.path() );
            }
        }

        return entries;
    }
}  // namespace

Explorer::Explorer( Window & window )
  : m_window { window },
    m_settings {},
    m_currentDirectory { ds::get_home_directory() },
    m_searchBox { m_currentDirectory.string() },
    m_previousDirectories {},
    m_nextDirectories {},
    m_table {},
    m_nbColumns { 4 }
{
    this->update_entries();
}

void Explorer::update()
{
    ImGuiWindowFlags fullScreenflags = ImGuiWindowFlags_NoDecoration
                                       | ImGuiWindowFlags_NoMove
                                       | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::SetNextWindowPos( ImGui::GetMainViewport()->Pos );
    ImGui::SetNextWindowSize( ImGui::GetMainViewport()->Size );
    ImGui::PushStyleColor( ImGuiCol_WindowBg, m_settings.backgroundColor );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
    if ( ImGui::Begin( "File Explorer", nullptr, fullScreenflags ) )
    {
        ImGui::PopStyleVar( 2 );
        this->update_header_bar();
        this->update_table_gui();
    }
    else
    {
        ImGui::PopStyleVar( 2 );
    }
    ImGui::End();
    ImGui::PopStyleColor();

    if ( m_settings.showDemoWindow )
    {
        ImGui::ShowDemoWindow( &m_settings.showDemoWindow );
    }
}

void Explorer::update_entries()
{
    // Get all entries to show in the current directory
    std::vector< fs::directory_entry > entries {};
    for ( auto const & entry : fs::directory_iterator( m_currentDirectory ) )
    {
        if ( ! ds::is_showed_gui( entry )
             || ( ! m_settings.showHidden && ds::is_hidden( entry ) ) )
        {
            continue;
        }
        entries.push_back( entry );
    }

    Trace::Debug( fmt::format( "Number of entries: {}", entries.size() ) );

    m_table.resize( boost::extents[entries.size()][m_nbColumns] );
    for ( std::size_t row = 0; row < entries.size(); ++row )
    {
        Trace::Debug( fmt::format( "row: {}", row ) );
        Trace::Debug(
            fmt::format( "Entry: {}", entries[row].path().string() ) );
        for ( std::size_t column = 0; column < m_nbColumns; column++ )
        {
            switch ( column )
            {
            case 0 :
                // Used internally to store the path
                m_table[row][column] = entries[row].path().string();
                break;
            case 1 :
                m_table[row][column] = entries[row].path().filename().string();
                break;
            case 2 :
                m_table[row][column] =
                    ds::get_size_pretty_print( entries[row] );
                break;
            case 3 :
                m_table[row][column] = ds::get_type( entries[row] );
                break;
            default :
                m_table[row][column] = "N/A";
                break;
            }
        }
    }
    Trace::Debug( "End of update_entries" );
}

void Explorer::change_directory( fs::path const & path )
{
    m_previousDirectories.push_back( m_currentDirectory );
    m_nextDirectories.clear();
    this->set_current_dir( path );
}

void Explorer::change_to_previous_dir()
{
    if ( ! m_previousDirectories.empty() )
    {
        this->add_to_next_dir( m_currentDirectory );
        this->set_current_dir( m_previousDirectories.back() );
        m_previousDirectories.pop_back();
    }
}

void Explorer::change_to_next_dir()
{
    if ( ! m_nextDirectories.empty() )
    {
        this->add_to_previous_dir( m_currentDirectory );
        this->set_current_dir( m_nextDirectories.back() );
        m_nextDirectories.pop_back();
    }
}

void Explorer::update_header_bar()
{
    if ( ImGui::Button( "Previous" ) )
    {
        this->change_to_previous_dir();
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Next" ) )
    {
        this->change_to_next_dir();
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Parent" ) )
    {
        this->change_directory( m_currentDirectory.parent_path() );
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Home" ) )
    {
        this->change_directory( ds::get_home_directory() );
    }
    ImGui::SameLine();
    this->update_search_box();
    ImGui::SameLine();
    if ( ImGui::Button( "Settings##SettingsButton" ) )
    {
        m_settings.showSettings = ! m_settings.showSettings;
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Refresh##RefreshButton" ) )
    {
        this->update_entries();
    }
    this->update_settings();
}

void Explorer::update_search_box()
{
    char currentDir[PATH_MAX];
    std::strcpy( currentDir, m_searchBox.c_str() );
    ImGui::InputText( "##Current Directory", currentDir, PATH_MAX );
    m_searchBox = currentDir;

    bool isInputTextPressed {
        ImGui::IsItemFocused()
        && ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_Enter ) ) };
    bool isInputTextActive { ImGui::IsItemActive() };

    if ( ImGui::IsItemActivated() )
    {
        ImGui::OpenPopup( "Entry Autocompletion",
                          ImGuiPopupFlags_NoOpenOverExistingPopup );
    }

    ImGui::SetNextWindowPos(
        ImVec2( ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y ) );
    if ( ImGui::BeginPopup(
             "Entry Autocompletion",
             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove
                 | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow ) )
    {
        for ( auto const & entry : filter_entry(
                  m_searchBox.parent_path(), m_searchBox.filename().string(),
                  m_settings.showHidden ) )
        {
            if ( ImGui::Selectable( entry.string().c_str() ) )
            {
                this->change_directory( entry );
                ImGui::CloseCurrentPopup();
            }
        }
        if ( isInputTextPressed
             || ( ! isInputTextActive && ! ImGui::IsWindowFocused() ) )
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if ( isInputTextPressed )
    {
        if ( fs::exists( m_searchBox ) )
        {
            this->open_entry( m_searchBox );
        }
    }
}

void Explorer::update_settings()
{
    if ( ! m_settings.showSettings )
    {
        return;
    }

    // position the settings window at the top right of the screen
    ImGui::SetNextWindowPos(
        ImVec2(
            ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x
                - ImGui::GetStyle().WindowPadding.x,
            ImGui::GetMainViewport()->Pos.y + ImGui::GetStyle().WindowPadding.y
                + ImGui::GetItemRectMax().y ),
        ImGuiCond_Always, ImVec2( 1.0f, 0.0f ) );
    ImGuiWindowFlags settingsFlags =
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_AlwaysAutoResize;

    ImGui::Begin( "Settings", &m_settings.showSettings, settingsFlags );

    ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
    if ( ImGui::BeginTabBar( "SettingsTab", tabBarFlags ) )
    {
        if ( ImGui::BeginTabItem( "Preferences" ) )
        {
            ImGui::Checkbox( "Show Hidden Files/Folder",
                             &m_settings.showHidden );
            ImGui::Checkbox( "Show Demo Window", &m_settings.showDemoWindow );
            if ( ImGui::Button( "Reset Preferences" ) )
            {
                m_settings.reset();
            }

            ImGui::EndTabItem();
        }
        if ( ImGui::BeginTabItem( "Window" ) )
        {
            window_configuration( m_window );
            ImGui::EndTabItem();
        }
        if ( ImGui::BeginTabItem( "Window Informations" ) )
        {
            information( m_window );
            ImGui::EndTabItem();
        }
        if ( ImGui::BeginTabItem( "Explorer Informations" ) )
        {
            this->update_debug();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void Explorer::update_table_gui()
{
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable
                            | ImGuiTableFlags_NoBordersInBodyUntilResize;

    ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2 { 0.f, 10.f } );
    if ( ImGui::BeginTable( "Filesystem Item List", 3, flags ) )
    {
        ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthStretch );
        ImGui::TableSetupColumn( "Size", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed );
        ImGui::TableHeadersRow();

        // Trace::Debug( fmt::format( "Table Size: {} {}",
        //                            m_currentDirectory.string(),
        //                            m_table.size() ) );

        std::optional< fs::path > selectedEntry { std::nullopt };

        std::size_t idxRow = 0;
        for ( auto const & row : m_table )
        {
            // Trace::Debug( fmt::format( "Current row: {}", idxRow ) );
            ImGui::TableNextRow( ImGuiTableRowFlags_None );

            std::size_t idxColumn = 0;
            for ( std::string const & cell : row )
            {
                if ( idxColumn == 0 )
                {
                    ++idxColumn;
                    continue;
                }

                ImGui::TableSetColumnIndex( idxColumn - 1 );

                ImGuiSelectableFlags selectable_flags =
                    ImGuiSelectableFlags_SpanAllColumns;
                // | ImGuiSelectableFlags_AllowItemOverlap;
                bool        isSelected { false };
                std::string id {
                    fmt::format( "{}##Cell{}-{}", cell, idxColumn, idxRow ) };
                ImGui::Selectable( id.c_str(), &isSelected, selectable_flags,
                                   ImVec2 { 0, 50.f } );

                if ( ImGui::IsItemHovered()
                     && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
                {
                    Trace::Debug( "Double Clicked: " + row[0] );
                    selectedEntry = fs::path { row[0] };
                    break;
                }
                ++idxColumn;
            }
            ++idxRow;
        }

        // Open the selected entry after the loop because we can't modify the
        // table while iterating over it
        if ( selectedEntry.has_value() )
        {
            this->open_entry( selectedEntry.value() );
        }
    }
    ImGui::PopStyleVar( 1 );
    ImGui::EndTable();
}

void Explorer::update_debug()
{
    ImGui::Text( "Current Directory : %s",
                 m_currentDirectory.string().c_str() );

    // Button to reset previous and next directories
    if ( ImGui::Button( "Reset Previous/Next Directories" ) )
    {
        m_previousDirectories.clear();
        m_nextDirectories.clear();
    }
    ImGui::Text( "Previous Directories :" );
    for ( auto const & dir : m_previousDirectories )
    {
        ImGui::Text( "%s", dir.string().c_str() );
    }
    ImGui::Separator();
    ImGui::Text( "Next Directories :" );
    for ( auto const & dir : m_nextDirectories )
    {
        ImGui::Text( "%s", dir.string().c_str() );
    }
}

void Explorer::add_to_previous_dir( fs::path const & path )
{
    m_previousDirectories.push_back( fs::path { path } );
    if ( m_previousDirectories.size() > m_settings.maxHistorySize )
    {
        m_previousDirectories.erase( m_previousDirectories.begin() );
    }
}

void Explorer::add_to_next_dir( fs::path const & path )
{
    // todo check if it's necessary to copy the path ?
    m_nextDirectories.push_back( fs::path { path } );
    if ( m_nextDirectories.size() > m_settings.maxHistorySize )
    {
        m_nextDirectories.erase( m_nextDirectories.begin() );
    }
}

void Explorer::set_current_dir( fs::path const & path )
{
    m_currentDirectory = path;
    m_searchBox        = m_currentDirectory;
    this->update_entries();
}

void Explorer::open_entry( fs::path const & entry )
{
    if ( fs::is_directory( entry ) )
    {
        this->change_directory( entry );
    }
    else
    {
        ds::open( entry );
    }
}
