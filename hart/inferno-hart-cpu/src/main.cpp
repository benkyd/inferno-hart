#include <inferno_hart.hpp>
#include <hart_graphics.hpp>

#include <tracing/ray.hpp>

#include <iostream>
#include <thread>

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

        std::cout << "INFERNO HART CPU RECIEVED " << vc / 3 << " VERTICIES AND " << ic / 3 << " INDICIES" << std::endl;
     
        mState = EModuleState::Ready;
    }
    
    void updateTris() override {}

    void intersectMasterWorker()
    {
        for (;;)
        {
            std::unique_lock<std::mutex> lock(_mData);
            if (mToTrace.empty())
            {
                lock.unlock();
                mState = EModuleState::Ready;
                continue;
            }
            mState = EModuleState::Trace;

            mToTrace.pop();
        }
    }

private:
    std::thread mMasterWorker;
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
