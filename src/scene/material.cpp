#include "material.hpp"

#include "preview_renderer/shader.hpp"

namespace inferno::scene {

Material::Material(std::string name)
    : mName(name)
{

}

Material::~Material()
{

}

void Material::setGlShader(graphics::Shader* shader)
{
    mGlShader = shader;
}

graphics::Shader* Material::getGlShader()
{
    return mGlShader;
}
}
