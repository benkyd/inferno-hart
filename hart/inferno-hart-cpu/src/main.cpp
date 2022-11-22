#include <inferno_hart.hpp>
#include <scene/mesh.hpp>

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
