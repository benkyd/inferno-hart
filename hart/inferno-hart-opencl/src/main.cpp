#include <inferno_hart.hpp>

#include <iostream>

using namespace inferno;

class HARTGPU : public ::HARTModule
{
public:
    HARTGPU()
    {
        std::cout << "Module HART GPU" << std::endl;
    }

    ~HARTGPU()
    {
        std::cout << "Goodbye Module HART GPU" << std::endl;
    }
    
    void submitTris(void* vert,
                    void* norm,
                    int vc,
                    void* indicies,
                    int ic) override {}
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
};

HART_INTERFACE void* _GET()
{
    return new HARTGPU;
}

HART_INTERFACE void _DESTROY(void* inst)
{
    HARTGPU* instance = (HARTGPU*)inst;
    delete instance;
}

HART_INTERFACE void* _CREDIT()
{
    return new ModuleCredit {
        .ModuleName = "HART_GPU",
        .AuthorName = "Ben Kyd",
        .ModuleDesc = "Accelerating inferno raytracing with OpenCL",
        .VersionMajor = 0,
        .VersionMinor = 0,
        .VersionBuild = 1,
    };
}
