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

//void Material::setGlShader(Shader* shader)
//{
    //mGlShader = shader;
//}

//Shader* Material::getGlShader()
//{
    //return mGlShader;
//}
}
