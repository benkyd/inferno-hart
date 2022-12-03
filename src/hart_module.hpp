#pragma once

// the HHM (Hamlin Hamlin McGill) aka the Head HART Module keeps track of the module
// and gives the renderer a cleaner interface to talk to a HART Module

#include "hart_directory.hpp"

namespace inferno {

class Scene;
class Ray;
class RayField;
class HitInfo;

class HARTModule;

class HHM 
{
public:
    HHM();
    ~HHM();

    HARTModuleDirectory* getModuleDirectory();

    void newScene(Scene* scene);
    void notifySceneUpdate();    

    void startTrace(RayField* sourceScatter);

    void rayReturn(HitInfo* hit);
    void bounce(Ray* newRay);

private:
    HARTModuleDirectory mDirectory;

};

}
