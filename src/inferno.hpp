#pragma once

#include "window.hpp"

namespace core {
class Inferno
{
public:
    Inferno(int argc, char** argv);
    int run();

private:
    Window* mWin = NULL;
};
}
