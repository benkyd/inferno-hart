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

void HHM::newScene(Scene* scene)
{
    // extract mesh's
}

void HHM::notifySceneUpdate()
{
    // same again
}

void HHM::startTrace(RaySource* sourceScatter)
{

}

void HHM::rayReturn(HitInfo* hit)
{

}

void HHM::bounce(Ray* newRay)
{

}

