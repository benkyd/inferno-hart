#pragma once

// the HHM (Hamlin Hamlin McGill) aka the Head HART Module keeps track of the module
// and gives the renderer a cleaner interface to talk to a HART Module

#include <vector>

#include <graphics.hpp>
#include "hart_directory.hpp"

namespace inferno {

class Scene;
class Ray;
class HitInfo;
class HARTModule;

class HHM 
{
public:
    HHM();
    ~HHM();

    HARTModuleDirectory* getModuleDirectory();
    EModuleState getModuleState();

    void newScene(Scene* scene);
    void notifySceneUpdate();    

    void rayReturn(HitInfo* hit);
    void bounce(Ray* newRay);

    void startTrace(RayField sourceScatter);

private:
    HARTModuleDirectory mDirectory;
};

}
