#include "hart_module.hpp"

#include <renderer/ray_source.hpp>
#include <renderer/renderer.hpp>

#include <scene/scene.hpp>
#include <scene/mesh.hpp>

#include <spdlog/spdlog.h>

#include <vector>

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

EModuleState HHM::getModuleState()
{
    HARTModule* mod = mDirectory.getActiveModule();
    return mod->getState();
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
        spdlog::debug("Mesh for module ready... {} {}", verticies, normals);
        mod->submitTris(verticies, normals, vertexCount, indicies, indexCount);
    }
}

void HHM::notifySceneUpdate()
{
    HARTModule* mod = mDirectory.getActiveModule();
    mod->updateTris();
}

void rayHitCallback(void* hhm, HitInfo* hit)
{
    ((HHM*)hhm)->rayReturn(hit);
}

void HHM::rayReturn(HitInfo* hit)
{
    HARTModule* mod = mDirectory.getActiveModule();
    spdlog::debug("HIT!!");
    Renderer->computeHit(hit);
}

void HHM::bounce(Ray* newRay)
{
    HARTModule* mod = mDirectory.getActiveModule();

}

void HHM::startTrace(RayField sourceScatter)
{
    HARTModule* mod = mDirectory.getActiveModule();
    mod->passContext((void*)this, &rayHitCallback);
    mod->submitQueue(sourceScatter);
}
