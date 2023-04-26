#pragma once

#include <optional>

#include "app/explorer_settings.hpp"  // for ExplorerSettings
#include "app/folder_navigator.hpp"   // for FolderNavigator
#include "app/window.hpp"             // for Window

class Explorer
{
  public:

  private:
    Window &         m_window;
    ExplorerSettings m_settings;

    std::vector< FolderNavigator > m_tabs;
    std::optional< unsigned int >  m_idxTab;

    bool m_showSettings;
    bool m_showDemoWindow;

  public:
    Explorer( Window & window );
    virtual ~Explorer() = default;

    void              update ();
    FolderNavigator & get_current_tab ();
    void              add_tab ( fs::path const & path );

  private:
    void update_header_bar ();
    void update_search_box ();
    void update_table_gui ();
    void update_settings ();

    void open_entry ( fs::path const & entry );
};
