#pragma once

#include <vector>  // for vector

#include <imgui/imgui.h>  // for ImVec4

#include "app/filesystem.hpp"  // for fs::path
#include "app/window.hpp"      // for Window

class Explorer
{
    bool m_showDemoWindow;
    bool m_showInfoWindow;
    bool m_showWindowConfig;
    bool m_showHidden;
    bool m_showDebugWindow;

    fs::path m_currentDirectory;
    fs::path m_searchBox;
    ImVec4   m_backgroundColor;

    std::vector< fs::path > m_previousDirectories;
    std::vector< fs::path > m_nextDirectories;
    unsigned int            m_maxHistorySize;

  public:
    Explorer();
    virtual ~Explorer() = default;

    void update ( Window & window );

    void change_directory ( fs::path const & path );
    void change_to_previous_dir ();
    void change_to_next_dir ();

  private:
    void update_table_gui ();
    void update_row_gui ( fs::directory_entry entry, int nbColumns, int row );
    void update_debug ();

    void add_to_previous_dir ( fs::path const & path );
    void add_to_next_dir ( fs::path const & path );

    void open_entry ( fs::directory_entry const & entry );
};
