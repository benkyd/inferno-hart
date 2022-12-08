#pragma once

#include <string>
#include <mutex>
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

HART_INTERFACE typedef void* (*HART_INIT_F)(void);
HART_INTERFACE typedef void (*HART_DESTROY_F)(void*);
HART_INTERFACE typedef void* (*HART_CREDIT_F)(void);

typedef void (*HART_HIT_CALLBACK)(void* context, HitInfo* hit);

// Module should set up it's worker in the constructor
// worker(s) pop items from mToTrace, intersect with
// programmer-defined structure from submitTris and calls
// Hit() with the context and the result of the trace
// FOR NOW: Rays and tri's are owned by Inferno, _NOT_ HART
class HARTModule
{
public:
    // Constructor & destructor is done in the module
    virtual void submitTris(void* vert, void* norm, int vc, void* indicies, int ic) = 0;
    virtual void updateTris() = 0;

    // module keeps queue reference
    inline void submitQueue(std::vector<Ray*> queue)
    {
        std::lock_guard<std::mutex> lock(_mData);
        for (const auto& e: queue)
            mToTrace.push(e);
    }

    inline void pushtoQueue(Ray* ray)
    {
        std::lock_guard<std::mutex> lock(_mData);
        mToTrace.push(ray);
    }

    inline void passContext(void* context, HART_HIT_CALLBACK callback)
    {
        std::lock_guard<std::mutex> lock(_mData);
        mCtx = context;
        Hit = callback;
    }

protected:
    void* mCtx;
    HART_HIT_CALLBACK Hit = nullptr;

protected:
    std::queue<Ray*> mToTrace;

    std::mutex _mData;
};
    
}
