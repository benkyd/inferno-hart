#include "inferno.hpp"
#include "spdlog/spdlog.h"

#include <exception>

int main(int argc, char** argv) 
{
    try 
    {
        core::Inferno inferno(argc, argv);
        return inferno.run();
    }
    catch (std::exception e) 
    {
        spdlog::error(e.what());
        return -1;
    }
}
