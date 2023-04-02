#pragma once

#include "SDL/SDL.h"

class Explorer
{
  public:
    Explorer();
    virtual ~Explorer() = default;

    void handle_input ( SDL_Event event );
    void render ();
};
