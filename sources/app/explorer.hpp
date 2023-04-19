#pragma once

#include <imgui/imgui.h>  // for ImVec4

#include "app/filesystem.hpp"  // for fs::path
#include "app/window.hpp"      // for Window

class Explorer
{
    Window const & m_window;

    bool m_showDemoWindow;
    bool m_showInfoWindow;
    bool m_showHidden;

    fs::path m_currentDirectory;
    ImVec4   m_backgroundColor;

  public:
    Explorer( Window const & window );
    virtual ~Explorer() = default;

    void update ();

    void update_table_gui ();
    void update_row_gui ( fs::directory_entry entry, int nbColumns, int row );
};
