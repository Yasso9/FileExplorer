#pragma once

#include <optional>

#include "app/explorer_settings.hpp"  // for ExplorerSettings
#include "app/folder_navigator.hpp"   // for FolderNavigator
#include "app/window.hpp"             // for Window

class TabNavigator
{
    std::vector< FolderNavigator > m_tabs;
    std::optional< unsigned int >  m_idxTab;

  public:
    TabNavigator();
    virtual ~TabNavigator() = default;

    void update_gui ();
    void debug_gui ();

    FolderNavigator & get_current ();
    void              add ( fs::path const & path, bool changeCurrent );
    void              remove ( unsigned int idx );
    void              set_current ( unsigned int idx );
};

class Explorer
{
    Window &     m_window;
    TabNavigator m_tabNavigator;

    bool m_showSettings;
    bool m_showDemoWindow;

  public:
    Explorer( Window & window );
    virtual ~Explorer() = default;

    void update ();

  private:
    void update_header_bar ();
    void update_search_box ();
    void update_settings ();
};
