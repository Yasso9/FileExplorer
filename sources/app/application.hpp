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

    void update ();
    void update_elements ();

    void should_run ( bool shouldRun );
    bool should_run () const;
};
