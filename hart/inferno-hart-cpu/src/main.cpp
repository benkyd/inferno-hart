#include <inferno_hart.hpp>

#include <iostream>

using namespace inferno;

extern "C" Accelerator* get()
{
    return new Accelerator;
}

extern "C" void destroy(Accelerator* inst)
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
