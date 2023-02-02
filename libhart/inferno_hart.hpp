#pragma once

#include <spdlog/spdlog.h>

#include <string>
#include <mutex>
#include <atomic>
#include <queue>

/**
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ infero HART modules ~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
 * Modules are registered at load time - instantiated when selected             *
 * _GET, _DESTROY & _CREDIT must be defined and return valid context's          *
 *                                                                              *
 * Inferno will first initialise the module and then wait for the Ready state.  *
 *                                                                              *
 * The possible states a module can be in are:                                  *
 *  - Bad (Not ready)                                                           *
 *  - Ready (Ready for a command or a scene)                                    *
 *  - Build (Scene is submitted and being processed)                            *
 *  - Trace (Tracing!)                                                          *
 *                                                                              *
 * Once the HHM dispatches a new scene to the module, it will wait until        *
 * the state is Done to dispatch work during scene building the modules         *
 * state must be Build.                                                         *
 *                                                                              *
 * Once the scene is ready and so is the trace, the HHM will start the tracing  *
 * state by pushing rays to the queue, the module must go through these and     *
 * for each ray, call Hit and pass the current context, this may resul          *
 * in Inferno to push another ray to the queue, the module will go until        *
 * empty or signaled to stop.                                                   *
 *                                                                              *
 * Once empty the module will switch to the Ready state again, so Inferno       *
 * can get the next frame ready and repeat.                                     *
 *                                                                              *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
*/

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

enum class EModuleState : uint8_t
{
    Bad,    // Not ready!
    Ready,  // Ready for command or scene
    Build,  // Scene is passed, optimisation is taking place
    Trace,  // Tracing!
};

class Ray;
class HitInfo;

HART_INTERFACE typedef void* (*HART_INIT_F)(void);
HART_INTERFACE typedef void  (*HART_DESTROY_F)(void*);
HART_INTERFACE typedef void* (*HART_CREDIT_F)(void);

typedef void (*HART_HIT_CALLBACK)(void* context, HitInfo* hit);

// FOR NOW: Rays and tri's are owned by Inferno, _NOT_ HART
class HARTModule
{
public:
    // Constructor & destructor is done in the module
    virtual void submitTris(void* vert, void* norm, int vc, void* indicies, int ic) = 0;
    virtual void updateTris() = 0;

    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void stop(bool interrupt) = 0;

    // module keeps queue reference
    inline void submitQueue(std::vector<Ray*> queue)
    {
        std::lock_guard<std::mutex> lock(_mData);
        // mState = EModuleState::Trace;
        for (const auto& e: queue)
            mToTrace.push(e);
        spdlog::info("[hartmodule] New trace queue: {}", mToTrace.size());  
        this->start();
    }

    inline void pushtoQueue(Ray* ray)
    {
        std::lock_guard<std::mutex> lock(_mData);
        mToTrace.push(ray);
    }

    inline void passContext(void* context, HART_HIT_CALLBACK callback)
    {
        spdlog::debug("[hartmodule] Recieved context");
        std::lock_guard<std::mutex> lock(_mData);
        mCtx = context;
        Hit = callback;
    }

    inline EModuleState getState()
    {
        return mState;
    }

protected:
    std::queue<Ray*> mToTrace;
    std::atomic<EModuleState> mState = EModuleState::Bad;
    std::atomic<EModuleState> mLastState = EModuleState::Bad;

    std::mutex _mData;

protected:
    void* mCtx;
    HART_HIT_CALLBACK Hit = nullptr;
};
    
}
