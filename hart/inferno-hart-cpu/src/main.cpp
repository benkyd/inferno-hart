#include <inferno_hart.hpp>
#include <hart_graphics.hpp>

#include <tracing/ray.hpp>

#include <iostream>

using namespace inferno;

class HARTCPU : public ::HARTModule
{
public:
    HARTCPU()
    {
        std::cout << "Module HART CPU" << std::endl;
    }

    ~HARTCPU()
    {
        std::cout << "Goodbye Module HART CPU" << std::endl;
    }

    void submitTris(void* vert,
                    void* norm,
                    int vc,
                    void* indicies,
                    int ic) override 
    {
        std::cout << "INFERNO HART CPU RECIEVED " << vc / 3 << " VERTICIES AND " << ic / 3 << " INDICIES" << std::endl;
    }
    
    void updateTris() override {}



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
