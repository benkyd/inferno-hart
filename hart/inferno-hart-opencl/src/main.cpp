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
    
    void submitTris(std::vector<glm::vec3>* vert,
                            std::vector<glm::vec3>* norm,
                            std::vector<Material*>* mats,
                            std::vector<int>* indicies) override {}
    void updateTris(std::vector<glm::vec3>* vert,
                            std::vector<glm::vec3>* norm,
                            std::vector<Material*>* mats,
                            std::vector<int>* indicies)  override {}

    void submitQueue(std::queue<Ray*> queue) override {}
    void pushtoQueue(Ray* ray) override {}
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
