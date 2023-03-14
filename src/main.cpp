#include "inferno.hpp"
#include "yolo/yolo.hpp"

#include <exception>

int main(int argc, char** argv)
{
    try
    {
        return inferno::Inferno::GetInstance().run();
    }
    catch (std::exception e)
    {
        yolo::error(e.what());
        return -1;
    }
}
