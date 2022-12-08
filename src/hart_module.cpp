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
    // TODO: This may not be the way i want to approach it
    // as submitTris should take maybe a mesh ID and then the mesh data
    // as it is now, submitTris assumes it's getting the whole scene
    // which would involve a lot of mesh copying (avoid!) if i were to chain them
    for (auto* mesh : meshs) {
        void* verticies; void* normals; void* indicies;
        int vertexCount = mesh->getVerticies(verticies, normals);
        int indexCount = mesh->getIndicies(indicies);
        mod->submitTris(verticies, normals, vertexCount, indicies, indexCount);
    }
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

