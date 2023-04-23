#include "explorer.hpp"

#include <climits>
#include <iostream>

#include <fmt/core.h>

#include "app/display.hpp"
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

    std::string to_lowercase ( std::string const & str )
    {
        std::string lowercase {};
        lowercase.reserve( str.size() );
        std::transform( str.begin(), str.end(), std::back_inserter( lowercase ),
                        [] ( unsigned char c ) { return std::tolower( c ); } );
        return lowercase;
    }

    std::vector< fs::path > filter_entry ( fs::path const &    directory,
                                           std::string const & entryFilter,
                                           bool                showHidden )
    {
        std::vector< fs::path > entries {};

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

            if ( to_lowercase( entry.path().filename().string() )
                     .find( to_lowercase( entryFilter ) )
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
    m_showSettings { false },
    m_showDemoWindow { false },
    m_showHidden { false },
    m_backgroundColor { 0.2f, 0.2f, 0.2f, 1.f },
    m_maxHistorySize { 15 },
    m_currentDirectory { ds::get_home_directory() },
    m_searchBox { m_currentDirectory.string() },
    m_previousDirectories {},
    m_nextDirectories {}
{}

void Explorer::update()
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
}

void Explorer::change_directory( fs::path const & path )
{
    m_previousDirectories.push_back( m_currentDirectory );
    m_nextDirectories.clear();

    m_currentDirectory = path;
    m_searchBox        = m_currentDirectory;
}

void Explorer::change_to_previous_dir()
{
    if ( ! m_previousDirectories.empty() )
    {
        this->add_to_next_dir( m_currentDirectory );
        m_currentDirectory = m_previousDirectories.back();
        m_searchBox        = m_currentDirectory;
        m_previousDirectories.pop_back();
    }
}

void Explorer::change_to_next_dir()
{
    if ( ! m_nextDirectories.empty() )
    {
        this->add_to_previous_dir( m_currentDirectory );
        m_currentDirectory = m_nextDirectories.back();
        m_searchBox        = m_currentDirectory;
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
        m_showSettings = ! m_showSettings;
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
        for ( auto const & entry :
              filter_entry( m_searchBox.parent_path(),
                            m_searchBox.filename().string(), m_showHidden ) )
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
            this->open_entry( fs::directory_entry { m_searchBox } );
        }
    }
}

void Explorer::update_settings()
{
    if ( ! m_showSettings )
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

    ImGui::Begin( "Settings", &m_showSettings, settingsFlags );

    if ( ImGui::BeginTabBar( "SettingsTab" ) )
    {
        if ( ImGui::BeginTabItem( "Preferences" ) )
        {
            ImGui::Checkbox( "Show Hidden Files/Folder", &m_showHidden );
            ImGui::Checkbox( "Show Demo Window", &m_showDemoWindow );
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

    if ( m_showDemoWindow )
    {
        ImGui::ShowDemoWindow( &m_showDemoWindow );
    }

    ImGui::End();
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

namespace
{
    std::string get_open_command ()
    {
        std::string command;
#if defined( _WIN32 )
        command = "start";
#elif defined( __APPLE__ )
        command = "open";
#elif defined( __linux__ )
        command = "xdg-open";
#else
        Trace::Error( "Unsupported operating system" );
#endif
        return command;
    }

    bool open_file ( fs::path const & file )
    {
        std::string command { get_open_command() + " \"" + file.string()
                              + "\"" };

        int result = std::system( command.c_str() );
        if ( result )
        {
            Trace::Error( "Error opening file with default program." );
            return false;
        }
        return true;
    }
}  // namespace

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
             && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
        {
            Trace::Debug( "Double Clicked: "
                          + entry.path().filename().string() );
            this->open_entry( entry );
        }
    }
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
    if ( m_previousDirectories.size() > m_maxHistorySize )
    {
        m_previousDirectories.erase( m_previousDirectories.begin() );
    }
}

void Explorer::add_to_next_dir( fs::path const & path )
{
    // todo check if it's necessary to copy the path ?
    m_nextDirectories.push_back( fs::path { path } );
    if ( m_nextDirectories.size() > m_maxHistorySize )
    {
        m_nextDirectories.erase( m_nextDirectories.begin() );
    }
}

void Explorer::open_entry( fs::directory_entry const & entry )
{
    if ( entry.is_directory() )
    {
        this->change_directory( entry.path() );
    }
    else
    {
        open_file( entry.path() );
    }
}
