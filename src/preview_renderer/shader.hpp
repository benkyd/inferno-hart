#pragma once

#include "../graphics.hpp"

#include <string>
#include <memory>
#include <vector>
#include <filesystem>
#include <unordered_map>

namespace inferno::graphics {

typedef struct ShaderPreprocessorDefinition {
    int start, end;
    std::string key;
    std::string def;
} shaderpreprocessordefinition;


typedef struct Shader {
    GLuint Shaders[3];
    GLuint Program;
    std::unordered_map<std::string, GLuint> Attributes;
    std::unordered_map<std::string, GLuint> Uniforms;
    std::vector<ShaderPreprocessorDefinition> PreprocessorDefinitions;
} Shader;

std::unique_ptr<Shader> shader_create();
void shader_cleanup(std::unique_ptr<Shader>& shader);

void shader_load(std::unique_ptr<Shader>& shader, std::filesystem::path path);
void shader_link(std::unique_ptr<Shader>& shader);
// TODO: Implement shader_reload
void shader_add_attribute(std::unique_ptr<Shader>& shader, const std::string& attribute);
void shader_add_uniform(std::unique_ptr<Shader>& shader, const std::string& uniform);
GLuint shader_get_attribute(std::unique_ptr<Shader>& shader, const std::string& attribute);
GLuint shader_get_uniform(std::unique_ptr<Shader>& shader, const std::string& uniform);

void shader_use(std::unique_ptr<Shader>& shader);
void shader_unuse(std::unique_ptr<Shader>& shader);

}

