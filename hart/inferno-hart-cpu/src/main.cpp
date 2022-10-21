#include <inferno_hart.hpp>

#include <iostream>

using namespace core::HART;

extern "C" core::HART::Accelerator* get()
{
    return new core::HART::Accelerator;
}

extern "C" void destroy(core::HART::Accelerator* inst)
{
    delete inst;
}

Accelerator::Accelerator()
{

} 

void Accelerator::Init()
{
        std::cout << "Shared Library!" << std::endl;
}
