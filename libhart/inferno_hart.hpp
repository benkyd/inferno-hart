#pragma once

#include <string>
#include <queue>

namespace inferno {

#ifdef _WIN32
#include <Windows.h>
#define HART_EXTENSION ".dll"
#define HART_INTERFACE extern "C" __declspec(dllexport)
#else // UNIX-Like
#include <dlfcn.h>
#define HART_EXTENSION ".so"
#define HART_INTERFACE extern "C"
#endif

struct ModuleCredit
{
    const std::string ModuleName;
    const std::string AuthorName;
    const std::string ModuleDesc;
    const int VersionMajor;
    const int VersionMinor;
    const int VersionBuild;
};

class Ray;
class HitInfo;
class Material;

HART_INTERFACE typedef void* (*HART_INIT_F)(void);
HART_INTERFACE typedef void (*HART_DESTROY_F)(void*);
HART_INTERFACE typedef void* (*HART_CREDIT_F)(void);

typedef void (*HART_HIT_CALLBACK)(HitInfo* hit);

class HARTModule
{
public:
    // Constructor & destructor is done in the module
    virtual void submitTris(void* vert,
                            void* norm,
                            std::vector<Material*>* mats,
                            std::vector<int>* indicies) = 0;
    virtual void updateTris(void* vert,
                            void* norm,
                            std::vector<Material*>* mats,
                            std::vector<int>* indicies) = 0;

    // module keeps queue reference
    virtual void submitQueue(std::vector<Ray*> queue) = 0;
    virtual void pushtoQueue(Ray* ray) = 0;

    inline void passHitCallback(HART_HIT_CALLBACK callback)
    {
        Hit = callback;
    }

private:
    HART_HIT_CALLBACK Hit = nullptr;

private:
    std::queue<Ray*> mToTrace;
};
    
}
