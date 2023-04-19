#pragma once

#include "app/window.hpp"

class Explorer
{
    Window const & m_window;

  public:
    Explorer( Window const & window );
    virtual ~Explorer() = default;

    void update ();
};
