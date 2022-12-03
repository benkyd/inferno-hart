#include "hart_module.hpp"

#include <vector>

#include <renderer/ray_source.hpp>
#include <scene/scene.hpp>
#include <scene/mesh.hpp>

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
    HARTModule* mod = mDirectory.getActiveModule();
    std::vector<Mesh*> meshs = scene->getRenderables();
    for (const auto* mesh : meshs) {
        // mesh->getVerticies();
    }
    // mod->submitTris();
}

void HHM::notifySceneUpdate()
{
    HARTModule* mod = mDirectory.getActiveModule();
    // same again
}

void HHM::startTrace(RayField sourceScatter)
{
    HARTModule* mod = mDirectory.getActiveModule();
    mod->submitQueue(sourceScatter);
}

void HHM::rayReturn(HitInfo* hit)
{
    HARTModule* mod = mDirectory.getActiveModule();

}

void HHM::bounce(Ray* newRay)
{
    HARTModule* mod = mDirectory.getActiveModule();

}

