#include "shader.hpp"

#include <fstream>
#include <iostream>

#include "yolo/yolo.hpp"

namespace inferno::graphics {

static std::unordered_map<GLuint, int> shader2Index = {};

inline std::string trim(std::string& str)
{
    str.erase(str.find_last_not_of(' ') + 1); // suffixing spaces
    str.erase(0, str.find_first_not_of(' ')); // prefixing spaces
    return str;
}

std::string textFromFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    return std::string((std::istreambuf_iterator<char>(input)),
        std::istreambuf_iterator<char>());
}

bool checkShader(GLuint uid)
{
    GLint isCompiled = 0;
    // glGetShaderiv(uid, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        // glGetShaderiv(uid, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        // glGetShaderInfoLog(uid, maxLength, &maxLength, &errorLog[0]);

        for (int i = 0; i < errorLog.size(); i++) {
            std::cout << errorLog[i];
        }

        // glDeleteShader(uid);
        return false;
    }

    return true;
}

Shader* shader_create(VkDevice device)
{
    Shader* shader = new Shader;
    shader->Device = device;

    return shader;
}

void shader_cleanup(Shader* shader)
{
    vkDestroyShaderModule(shader->Device, shader->VertexShader, nullptr);
    vkDestroyShaderModule(shader->Device, shader->FragmentShader, nullptr);
}

void shader_load(Shader* shader, std::filesystem::path path)
{
    assert(std::filesystem::exists(path));

    // path is the filename, code needs to add .vert.spv or .frag.spv
    // std::string shaderPath = "shaders/" + path + ".spv";

    std::string shaderPath = path.string();
    std::string vertexShaderPath = shaderPath + ".vert.spv";
    std::string fragmentShaderPath = shaderPath + ".frag.spv";
    assert(std::filesystem::exists(vertexShaderPath));
    assert(std::filesystem::exists(fragmentShaderPath));

    std::string vertexLoadedShaderCode = textFromFile(vertexShaderPath);
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = vertexLoadedShaderCode.size();
    createInfo.pCode
        = reinterpret_cast<const uint32_t*>(vertexLoadedShaderCode.data());

    if (vkCreateShaderModule(
            shader->Device, &createInfo, nullptr, &shader->VertexShader)
        != VK_SUCCESS) {
        yolo::error("failed to create shader module");
    }

    shader->ShaderStages[0].sType
        = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader->ShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader->ShaderStages[0].module = shader->VertexShader;
    shader->ShaderStages[0].pName = "main";

    std::string fragmentLoadedShaderCode = textFromFile(fragmentShaderPath);
    VkShaderModuleCreateInfo createInfo2 = {};
    createInfo2.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo2.codeSize = fragmentLoadedShaderCode.size();
    createInfo2.pCode
        = reinterpret_cast<const uint32_t*>(fragmentLoadedShaderCode.data());

    if (vkCreateShaderModule(
            shader->Device, &createInfo2, nullptr, &shader->FragmentShader)
        != VK_SUCCESS) {
        yolo::error("failed to create shader module");
    }

    shader->ShaderStages[1].sType
        = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader->ShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader->ShaderStages[1].module = shader->FragmentShader;
    shader->ShaderStages[1].pName = "main";
}

// void shader_add_attribute(Shader* shader, const std::string& attribute)
// {
//     // shader->Attributes[attribute] = glGetAttribLocation(shader->Program,
//     attribute.c_str());
// }
//
// void shader_add_uniform(Shader* shader, const std::string& uniform)
// {
//     shader->Uniforms[uniform] = glGetUniformLocation(shader->Program,
//     uniform.c_str());
// }
//
// GLuint shader_get_attribute(Shader* shader, const std::string& attribute)
// {
//     return shader->Attributes[attribute];
// }
//
// GLuint shader_get_uniform(Shader* shader, const std::string& uniform)
// {
//     return shader->Uniforms[uniform];
// }

void shader_use(Shader* shader)
{
    // glUseProgram(shader->Program);
}

void shader_unuse(Shader* shader)
{
    // glUseProgram(0);
}
}
