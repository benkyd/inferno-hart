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
            rootNode = nullptr;
        }

        ~HARTCPU()
        {
            this->stop(true);
            mMasterWorker.detach();
            if (rootNode) {
                deleteKDTree(rootNode);
            }
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

            std::vector<glm::vec3> vertices(mVc / 3);
            for (size_t i = 0; i < vertices.size(); ++i) {
                vertices[i] = glm::vec3(mVert[i * 3], mVert[i * 3 + 1], mVert[i * 3 + 2]);
            }

            std::vector<Triangle> triangles(mIc / 3);
            for (size_t i = 0; i < triangles.size(); ++i) {
                triangles[i].indices[0] = mIndices[i * 3];
                triangles[i].indices[1] = mIndices[i * 3 + 1];
                triangles[i].indices[2] = mIndices[i * 3 + 2];
            }

            rootNode = buildKDTree(vertices, triangles);
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
                // ...
                // (Keep the existing implementation of intersectMasterWorker, but replace the KDTree intersection part)

                std::vector<Triangle> candidateTriangles;
                intersectKDTree(rootNode, ray->Origin, ray->Direction, candidateTriangles);

                for (const Triangle& triangle : candidateTriangles)
                {
                    uint32_t ind1 = triangle.indices[0];
                    uint32_t ind2 = triangle.indices[1];
                    uint32_t ind3 = triangle.indices[2];

                    const glm::vec3 a = { mVert[ind1 * 3], mVert[ind1 * 3 + 1], mVert[ind1 * 3 + 2] };
                    const glm::vec3 b = { mVert[ind2 * 3], mVert[ind2 * 3 + 1], mVert[ind2 * 3 + 2] };
                    const glm::vec3 c = { mVert[ind3 * 3], mVert[ind3 * 3 + 1], mVert[ind3 * 3 + 2] };

                    // Perform intersection test...
                    if (!glm::intersectRayTriangle(ray->Origin, ray->Direction, a, b, c, coords, dist)) { continue; }
                    if (dist > bestDist || dist < 0.0f) { continue; }

                    //bestIdx = ;
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
        KDNode* rootNode;

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

