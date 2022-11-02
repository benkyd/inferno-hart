#pragma once

#include <string>

namespace inferno {

class Shader;

class Material {
public:
    Material(std::string name);
    ~Material();

    std::string getName();
    void setGlShader(Shader* shader);
    Shader* getGlShader();

private:
    std::string mName;
    Shader* mGlShader;
};

}
