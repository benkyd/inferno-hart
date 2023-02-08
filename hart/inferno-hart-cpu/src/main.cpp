#include <inferno_hart.hpp>
#include <hart_graphics.hpp>

#include <tracing/ray.hpp>
#include <tracing/hit.hpp>

#include <iostream>
#include <thread>
#include <chrono>

using namespace inferno;

class HARTCPU : public HARTModule
{
public:
    HARTCPU()
    {
        mMasterWorker = std::thread(&HARTCPU::intersectMasterWorker, this);
    }

    ~HARTCPU()
    {
        mMasterWorker.detach();
    }

    void submitTris(void* vert,
                    void* norm,
                    int vc,
                    void* indicies,
                    int ic) override 
    {
        mState = EModuleState::Build;

        mVert = (float*)vert; mNorm = (float*)norm; mVc = vc; mIndicies = (uint32_t*)indicies; mIc = ic;
        spdlog::info("[hartcpu] Recieved {} verticies ({}) and {} indicies ({})", vc / 3, vert, ic / 3, indicies);
        mState = EModuleState::Ready;
    }
    
    void updateTris() override {}

    void start() override 
    {
        
    }
    
    void pause() override
    {
    }

    void stop(bool interrupt) override
    {
    }

    void intersectMasterWorker()
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(_mData);
            mLastState = mState.load();
            if (mToTrace.size() == 0)
            {
                lock.unlock();
                mState = EModuleState::Ready;
                continue;
            }
            mState = EModuleState::Trace;

            if (mLastState != mState)
            {
                spdlog::info("[hartcpu] Signal start");
            }

            Ray* ray = mToTrace.front();
            int bestIdx = -1;
            glm::vec2 coords;
            glm::vec2 bestTexcoord;
            float bestDist = INFINITY;
            float dist;
            for (int i = 0; i < mIc; i += 9)
            {
                uint32_t ind = mIndicies[i];
                // Check if the ray intersects
                const glm::vec3 a = { mVert[ind + 1], mVert[ind + 2], mVert[ind + 3] };
                const glm::vec3 b = { mVert[ind + 4], mVert[ind + 5], mVert[ind + 6] };
                const glm::vec3 c = { mVert[ind + 7], mVert[ind + 8], mVert[ind + 9] };
            
                if (!glm::intersectRayTriangle(ray->Origin, ray->Direction, a, b, c, coords, dist)) { continue; }
                if (dist > bestDist || dist < 0.0f) { continue; }
                bestIdx = i;
                bestDist = dist;
                bestTexcoord = coords;
            }
            
            HitInfo hit;
            hit.Caller = ray;
            // If no hit, we still need to inform the HHM
            if (bestIdx < 0) 
            {
                mToTrace.pop();
                continue;
            }

            hit.Distance = bestDist;
            hit.UV = bestTexcoord;

            Hit(mCtx, &hit);

            mToTrace.pop();
        }
    }

private:
    std::condition_variable _mSignal;
    std::mutex _mStopperSignal;
    std::thread mMasterWorker;
    
private:  
    float* mVert;
    float* mNorm;
    int mVc;
    uint32_t* mIndicies;
    int mIc;
};

HART_INTERFACE void* _GET()
{
    return new HARTCPU;
}

HART_INTERFACE void _DESTROY(void* inst)
{
    HARTCPU* instance = (HARTCPU*)inst;
    delete instance;
}

HART_INTERFACE void* _CREDIT()
{
    return new ModuleCredit {
        .ModuleName = "HART_CPU",
        .AuthorName = "Ben Kyd",
        .ModuleDesc = "Accelerating inferno raytracing with CPU",
        .VersionMajor = 0,
        .VersionMinor = 0,
        .VersionBuild = 1,
    };
}
