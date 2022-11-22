#include "hart_module.hpp"

using namespace inferno;

HHM::HHM() 
    : mDirectory()
{
    mDirectory.discoverModules("./hart/", true);   
}

HHM::~HHM()
{

}

HARTModuleDirectory* HHM::getModuleDirectory()
{
    return &mDirectory;
}

