#pragma once

#include "window.hpp"

namespace core {
class Inferno
{
public:
    Inferno(int argc, char** argv);
    int run();
    void uiPreset();

private:
    Window* mWin = NULL;
};
}
