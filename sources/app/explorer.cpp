#include "explorer.hpp"

#include <algorithm>
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

TabNavigator::TabNavigator() : m_tabs {}, m_idxTab { std::nullopt } {}

void TabNavigator::update_gui()
{
    ImGuiTabBarFlags tabBarFlags =
        ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton;
    if ( ImGui::BeginTabBar( "ExplorerTab", tabBarFlags ) )
    {
        if ( ImGui::TabItemButton( "+", ImGuiTabItemFlags_Trailing
                                            | ImGuiTabItemFlags_NoTooltip ) )
        {
            Trace::Debug( "Add tab" );
            this->add( ds::get_home_directory(), true );
        }

        std::optional< std::size_t > idxTabToRemove = std::nullopt;
        for ( std::size_t idx = 0; idx < m_tabs.size(); ++idx )
        {
            ImGuiTabItemFlags tabItemFlags = ImGuiTabItemFlags_None;
            if ( m_idxTab.value() == idx )
            {
                tabItemFlags |= ImGuiTabItemFlags_SetSelected;
            }
            bool        isOpen = true;
            std::string label  = fmt::format(
                "{}##{}", m_tabs[idx].get_directory().filename().string(),
                idx );
            if ( ImGui::BeginTabItem( label.c_str(), &isOpen, tabItemFlags ) )
            {
                if ( m_idxTab.value() != idx )
                {
                    Trace::Debug( "Change tab :" + idx );
                    this->set_current( idx );
                }
                m_tabs[idx].update_gui();
                // this->get_current().update_gui();
                ImGui::EndTabItem();
            }
            if ( ! isOpen && m_tabs.size() > 1 )
            {
                idxTabToRemove = idx;
            }
        }
        if ( idxTabToRemove.has_value() )
        {
            Trace::Debug( "Remove tab" );
            this->remove( idxTabToRemove.value() );
        }

        ImGui::EndTabBar();
    }
}

void TabNavigator::debug_gui()
{
    if ( m_idxTab.has_value() )
    {
        ImGui::Text( "Current tab: %u", m_idxTab.value() );
    }
    else
    {
        ImGui::Text( "No tab selected" );
    }

    ImGui::Text( "Number of tabs : %lu", m_tabs.size() );
}

FolderNavigator & TabNavigator::get_current()
{
    if ( ! m_idxTab.has_value() )
    {
        Trace::Error( "No tab selected" );
    }
    return m_tabs[m_idxTab.value()];
}

void TabNavigator::add( fs::path const & path, bool changeCurrent )
{
    m_tabs.push_back( FolderNavigator { path } );
    if ( ! m_idxTab.has_value() || changeCurrent )
    {
        m_idxTab = m_tabs.size() - 1;
    }
}

void TabNavigator::remove( unsigned int idx )
{
    if ( idx >= m_tabs.size() )
    {
        Trace::Error( "Invalid tab index" );
    }

    m_tabs.erase( m_tabs.begin() + idx );

    if ( m_tabs.empty() )
    {
        m_idxTab = std::nullopt;
    }
    else if ( m_idxTab >= idx )
    {
        m_idxTab = m_idxTab.value() - 1;
    }

    Trace::Debug( fmt::format( "idx after rm: {}", m_idxTab.value() ) );
}

void TabNavigator::set_current( unsigned int idx )
{
    if ( idx >= m_tabs.size() )
    {
        Trace::Error( "Invalid tab index" );
        return;
    }

    m_idxTab = idx;
}

Explorer::Explorer( Window & window )
  : m_window { window },
    m_tabNavigator {},
    m_showSettings { false },
    m_showDemoWindow { false }
{
    m_tabNavigator.add( ds::get_home_directory(), true );
}

void Explorer::update()
{
    ImGuiWindowFlags fullScreenflags = ImGuiWindowFlags_NoDecoration
                                       | ImGuiWindowFlags_NoMove
                                       | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::SetNextWindowPos( ImGui::GetMainViewport()->Pos );
    ImGui::SetNextWindowSize( ImGui::GetMainViewport()->Size );
    ImGui::PushStyleColor( ImGuiCol_WindowBg,
                           Settings::get_instance().backgroundColor );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
    if ( ImGui::Begin( "File Explorer", nullptr, fullScreenflags ) )
    {
        ImGui::PopStyleVar( 2 );
        this->update_header_bar();

        m_tabNavigator.update_gui();
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

void Explorer::update_header_bar()
{
    if ( ImGui::Button( "Previous" ) )
    {
        m_tabNavigator.get_current().change_to_previous_dir();
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Next" ) )
    {
        m_tabNavigator.get_current().change_to_next_dir();
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Parent" ) )
    {
        // this->change_directory( m_currentDirectory.parent_path() );
        m_tabNavigator.get_current().to_parent_dir();
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Home" ) )
    {
        m_tabNavigator.get_current().change_directory(
            ds::get_home_directory() );
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
        m_tabNavigator.get_current().refresh();
    }
    this->update_settings();
}

void Explorer::update_search_box()
{
    std::string searchBoxStr {
        m_tabNavigator.get_current().get_search_box().string() };
    ImGui::InputText( "##Current Directory", &searchBoxStr );
    m_tabNavigator.get_current().set_search_box( searchBoxStr );

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
                  m_tabNavigator.get_current().get_search_box().parent_path(),
                  m_tabNavigator.get_current()
                      .get_search_box()
                      .filename()
                      .string(),
                  Settings::get_instance().showHidden ) )
        {
            if ( ImGui::Selectable( entry.string().c_str() ) )
            {
                m_tabNavigator.get_current().change_directory( entry );
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
        if ( fs::exists( m_tabNavigator.get_current().get_search_box() ) )
        {
            m_tabNavigator.get_current().open_entry(
                m_tabNavigator.get_current().get_search_box() );
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
                             &Settings::get_instance().showHidden );
            ImGui::Checkbox( "Show Demo Window", &m_showDemoWindow );
            if ( ImGui::Button( "Reset Preferences" ) )
            {
                Settings::get_instance().reset();
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
        if ( ImGui::BeginTabItem( "Tabs Informations" ) )
        {
            m_tabNavigator.debug_gui();
            ImGui::EndTabItem();
        }
        if ( ImGui::BeginTabItem( "Folder Informations" ) )
        {
            m_tabNavigator.get_current().gui_info();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}
