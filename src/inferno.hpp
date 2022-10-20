#pragma once

#include "window.hpp"

namespace inferno 
{
class InfernoHART
{
public:
    InfernoHART(int argc, char** argv);
    int run();

private:
    Window* win = NULL;
};
}
