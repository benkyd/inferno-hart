#pragma once

// the HHM (Hamlin Hamlin McGill) aka the Head HART Module keeps track of the module
// and gives the renderer a cleaner interface to talk to a HART Module

namespace inferno {

class Scene;
class Ray;
class RaySouce;
class HitInfo;

class HARTModule;

class HHM 
{
public:
    HHM();
    ~HHM();

    // needs to syncronusly stop the module's execution and
    // prepare for setting up a new HART layer
    void notifyModuleChange(HARTModule* newModule);

    void newScene(Scene* scene);
    void notifySceneUpdate();    

    void startTrace(RaySouce* sourceScatter);

    void rayReturn(HitInfo* hit);
    void bounce(Ray* newRay);

private:
    HARTModule* activeModule;

};

}
