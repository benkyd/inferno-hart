#include <inferno_hart.hpp>
#include <hart_graphics.hpp>

#include <tracing/ray.hpp>
#include <tracing/hit.hpp>

#include "kdtree.hpp"

#include <iostream>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <numeric>

using namespace inferno;

class HARTCPU : public HARTModule
{
public:
    HARTCPU()
    {
        mMasterWorker = std::thread(&HARTCPU::intersectMasterWorker, this);
        mLogModule = yolo::registerModule("hartcpu", "\u001b[35;1m");
    }

    ~HARTCPU()
    {
        this->stop(true);
        mMasterWorker.detach();
    }

    void submitTris(void* vert,
                    void* norm,
                    int vc,
                    void* indices,
                    int ic) override
    {
        std::lock_guard<std::mutex> lock(_mData);

        mState = EModuleState::Build;
        mVert = (float*)vert; mNorm = (float*)norm; mVc = vc; mIndices = (uint32_t*)indices; mIc = ic;
        yolo::info(mLogModule, "Recieved {} verticies ({}) and {} indicies ({})", vc / 3, vert, ic / 3, indices);

        std::vector<uint32_t> indicesToProcess(ic / 3);
        for (uint32_t i = 0; i < ic / 3; ++i)
        {
            indicesToProcess[i] = i;
        }

        mKdTree = new KDTree(mVert, mIndices, indicesToProcess, 0, indicesToProcess.size() - 1, 10);
        mKdTree->printTree(mKdTree->getRoot(), 1);
        yolo::info(mLogModule, "Accelerator ready..");

        mState = EModuleState::Idle;
    }

    void updateTris() override {}

    void start() override
    {
        std::lock_guard<std::mutex> signalLock(_mSignalMut);
        mIsRunning = true;
        mState = EModuleState::Trace;
        _mSignalCv.notify_all();

        yolo::info(mLogModule, "Signal master to start");

        {
            std::unique_lock<std::mutex> doneLock(_mDoneMut);
            _mDoneCv.wait(doneLock, [this] { return mState == EModuleState::Idle; });
        }
    }

    void stop(bool interrupt) override
    {
        if (!interrupt)
        {
            mIsRunning = false;
            return;
        }
        // TODO: Find a way to force the thread to hault
    }

    void intersectMasterWorker()
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(_mData);
            if (!mIsRunning)
            {
                _mSignalCv.wait(lock, [this]{ return (mIsRunning || mState == EModuleState::Trace); });
            }

            if (mToTrace.size() == 0)
            {
                lock.unlock();
                mState = EModuleState::Idle;
                _mDoneCv.notify_all();
                continue;
            }
            mState = EModuleState::Trace;

            Ray* ray = mToTrace.front();
            int bestIdx = -1;
            glm::vec2 coords;
            glm::vec2 bestTexcoord;
            float bestDist = INFINITY;
            float dist;

            // Traverse the K-D tree to identify the set of triangles that may intersect the ray.
            std::vector<uint32_t> candidateIndices;
            mKdTree->intersect(ray, candidateIndices);

            for (uint32_t idx : candidateIndices)
            {
                uint32_t ind1 = mIndices[idx * 3];
                uint32_t ind2 = mIndices[idx * 3 + 1];
                uint32_t ind3 = mIndices[idx * 3 + 2];

                const glm::vec3 a = { mVert[ind1], mVert[ind1 + 1], mVert[ind1 + 2] };
                const glm::vec3 b = { mVert[ind2], mVert[ind2 + 1], mVert[ind2 + 2] };
                const glm::vec3 c = { mVert[ind3], mVert[ind3 + 1], mVert[ind3 + 2] };

                // Perform intersection test...
                if (!glm::intersectRayTriangle(ray->Origin, ray->Direction, a, b, c, coords, dist)) { continue; }
                if (dist > bestDist || dist < 0.0f) { continue; }

                bestIdx = idx;
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
    // Signaling Stuffs
    std::atomic<bool> mIsRunning;
    std::thread mMasterWorker;
    std::mutex _mSignalMut;
    std::mutex _mDoneMut;
    std::condition_variable _mSignalCv;
    std::condition_variable _mDoneCv;

private:
    // Scene Data
    KDTree* mKdTree;

    float* mVert;
    float* mNorm;
    int mVc;
    uint32_t* mIndices;
    int mIc;

    uint8_t mLogModule;
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
