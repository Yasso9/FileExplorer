#pragma once

#include <imgui/imgui.h>  // for ImVec4

#include "tools/singleton.hpp"

class ExplorerSettings : public Singleton< ExplorerSettings >
{
    ENABLE_SINGLETON( ExplorerSettings );

  public:
    bool         showHidden;
    ImVec4       backgroundColor;
    unsigned int maxHistorySize;

  private:
    ExplorerSettings();
    virtual ~ExplorerSettings() = default;

  public:
    void reset ();
};

using Settings = ExplorerSettings;
