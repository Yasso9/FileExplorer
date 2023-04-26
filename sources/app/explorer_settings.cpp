#include "explorer_settings.hpp"

ExplorerSettings::ExplorerSettings()
{
    this->reset();
}

void ExplorerSettings::reset()
{
    showHidden      = false;
    backgroundColor = ImVec4( 0.2f, 0.2f, 0.2f, 1.f );
    maxHistorySize  = 15u;
}
