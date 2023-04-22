#include "inferno.hpp"
#include "yolo/yolo.hpp"

#include <exception>

int main(int argc, char** argv)
{
    auto* inferno = inferno::core::create_inferno();
    return inferno::core::run(inferno);
}

