#pragma once

#include <vector>  // for vector

#include <boost/multi_array.hpp>
#include <imgui/imgui.h>  // for ImVec4

#include "app/filesystem.hpp"  // for fs::path
#include "app/window.hpp"      // for Window

class Explorer
{
    Window & m_window;

    // todo put these attributes in a config struct
    bool         m_showSettings;
    bool         m_showDemoWindow;
    bool         m_showHidden;
    bool         m_showDebugWindow;
    ImVec4       m_backgroundColor;
    unsigned int m_maxHistorySize;

    fs::path m_currentDirectory;
    fs::path m_searchBox;

    std::vector< fs::path > m_previousDirectories;
    std::vector< fs::path > m_nextDirectories;

    boost::multi_array< std::string, 2 > m_table;
    unsigned int                         m_nbColumns;

  public:
    Explorer( Window & window );
    virtual ~Explorer() = default;

    void update ();
    void update_entries ();

    void change_directory ( fs::path const & path );
    void change_to_previous_dir ();
    void change_to_next_dir ();

  private:
    void update_header_bar ();
    void update_search_box ();
    void update_table_gui ();
    void update_settings ();
    void update_debug ();

    void add_to_previous_dir ( fs::path const & path );
    void add_to_next_dir ( fs::path const & path );
    void set_current_dir ( fs::path const & path );

    void open_entry ( fs::path const & entry );
};
