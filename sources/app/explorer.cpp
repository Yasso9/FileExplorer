#include "explorer.hpp"

#include <climits>
#include <iostream>

#include <fmt/core.h>
#include <imgui/imgui_stdlib.h>  // for ImGui::InputText

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
    m_tabs {},
    m_idxTab { std::nullopt },
    m_showSettings { false },
    m_showDemoWindow { false }
{
    this->add_tab( ds::get_home_directory() );
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

    if ( m_showDemoWindow )
    {
        ImGui::ShowDemoWindow( &m_showDemoWindow );
    }
}

FolderNavigator & Explorer::get_current_tab()
{
    if ( ! m_idxTab.has_value() )
    {
        Trace::Error( "No tab selected" );
    }
    return m_tabs[m_idxTab.value()];
}

void Explorer::add_tab( fs::path const & path )
{
    m_tabs.emplace_back( FolderNavigator { path, m_settings } );
    m_idxTab = m_tabs.size() - 1;
}

void Explorer::update_header_bar()
{
    if ( ImGui::Button( "Previous" ) )
    {
        this->get_current_tab().change_to_previous_dir();
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Next" ) )
    {
        this->get_current_tab().change_to_next_dir();
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Parent" ) )
    {
        // this->change_directory( m_currentDirectory.parent_path() );
        this->get_current_tab().to_parent_dir();
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Home" ) )
    {
        this->get_current_tab().change_directory( ds::get_home_directory() );
    }
    ImGui::SameLine();
    this->update_search_box();
    ImGui::SameLine();
    if ( ImGui::Button( "Settings##SettingsButton" ) )
    {
        m_showSettings = ! m_showSettings;
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Refresh##RefreshButton" ) )
    {
        this->get_current_tab().refresh();
    }
    this->update_settings();
}

void Explorer::update_search_box()
{
    std::string searchBoxStr {
        this->get_current_tab().get_search_box().string() };
    ImGui::InputText( "##Current Directory", &searchBoxStr );
    this->get_current_tab().set_search_box( searchBoxStr );

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
                  this->get_current_tab().get_search_box().parent_path(),
                  this->get_current_tab().get_search_box().filename().string(),
                  m_settings.showHidden ) )
        {
            if ( ImGui::Selectable( entry.string().c_str() ) )
            {
                this->get_current_tab().change_directory( entry );
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
        if ( fs::exists( this->get_current_tab().get_search_box() ) )
        {
            this->open_entry( this->get_current_tab().get_search_box() );
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

    ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
    if ( ImGui::BeginTabBar( "SettingsTab", tabBarFlags ) )
    {
        if ( ImGui::BeginTabItem( "Preferences" ) )
        {
            ImGui::Checkbox( "Show Hidden Files/Folder",
                             &m_settings.showHidden );
            ImGui::Checkbox( "Show Demo Window", &m_showDemoWindow );
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
            this->get_current_tab().gui_info();
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
        for ( auto const & row : this->get_current_tab().get_structure() )
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

void Explorer::open_entry( fs::path const & entry )
{
    if ( fs::is_directory( entry ) )
    {
        this->get_current_tab().change_directory( entry );
    }
    else
    {
        ds::open( entry );
    }
}
