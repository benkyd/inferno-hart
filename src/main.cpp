#include "inferno.hpp"
#include "yolo/yolo.hpp"

#include <exception>

int main(int argc, char** argv)
{
    auto* inferno = inferno::inferno_create();
    return inferno::inferno_run(inferno);
}

