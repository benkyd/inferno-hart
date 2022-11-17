#include <inferno_hart.hpp>
#include <scene/mesh.hpp>

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
