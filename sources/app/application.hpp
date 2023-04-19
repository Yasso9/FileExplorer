#pragma once

#include "app/explorer.hpp"
#include "app/window.hpp"

class Application
{
    Window m_window;
    bool   m_shouldRun;

    Explorer m_explorer;

  public:
    Application();
    virtual ~Application() = default;

    void handle_event ();
    void render ();

    void should_run ( bool shouldRun );
    bool should_run ();

    // todo Rename with get_window()
    Window const & window () const;
    Window &       window ();
};
