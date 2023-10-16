#pragma once

#include "../graphics.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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

Shader* shader_create();
void shader_cleanup(Shader* shader);

void shader_load(Shader* shader, std::filesystem::path path);
void shader_link(Shader* shader);

GLuint shader_get_program(Shader* shader);

// TODO: Implement shader_reload
void shader_add_attribute(Shader* shader, const std::string& attribute);
void shader_add_uniform(Shader* shader, const std::string& uniform);
GLuint shader_get_attribute(Shader* shader, const std::string& attribute);
GLuint shader_get_uniform(Shader* shader, const std::string& uniform);

void shader_use(Shader* shader);
void shader_unuse(Shader* shader);

}
