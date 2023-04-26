#pragma once

#include <imgui/imgui.h>  // for ImVec4

struct ExplorerSettings
{
    bool         showHidden;
    ImVec4       backgroundColor;
    unsigned int maxHistorySize;

    ExplorerSettings();
    virtual ~ExplorerSettings() = default;

    void reset ();
};
