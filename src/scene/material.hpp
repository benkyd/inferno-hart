#pragma once

#include <string>

#include <hart_graphics.hpp>

namespace inferno::graphics {
    class Shader;
};

namespace inferno::scene {

class HitInfo;

class Material {
public:
    Material(std::string name);
    ~Material();

    std::string getName();
    void setGlShader(graphics::Shader* shader);
    graphics::Shader* getGlShader();

    glm::vec3 sample(HitInfo* hit);

private:
    std::string mName;
    graphics::Shader* mGlShader;
};

}
