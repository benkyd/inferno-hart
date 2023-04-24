#pragma once

#include <string>

#include <hart_graphics.hpp>

namespace inferno {

//class Shader;
class HitInfo;

class Material {
public:
    Material(std::string name);
    ~Material();

    std::string getName();
    //void setGlShader(Shader* shader);
    //Shader* getGlShader();

    glm::vec3 sample(HitInfo* hit);

private:
    std::string mName;
    //Shader* mGlShader;
};

}
