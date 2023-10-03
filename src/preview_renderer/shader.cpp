#include "shader.hpp"

#include <fstream>
#include <iostream>

namespace inferno::graphics {

static std::unordered_map<GLuint, int> shader2Index = {
    { GL_VERTEX_SHADER, 0 },
    { GL_GEOMETRY_SHADER, 1 },
    { GL_FRAGMENT_SHADER, 2 }
};

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

std::vector<const ShaderPreprocessorDefinition*> getKeys(Shader* shader, std::string key)
{
    std::vector<const ShaderPreprocessorDefinition*> ret;
    for (const auto& p : shader->PreprocessorDefinitions)
        if (p.key == key)
            ret.push_back(&p);
    return ret;
}

bool checkShader(GLuint uid)
{
    GLint isCompiled = 0;
    glGetShaderiv(uid, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(uid, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(uid, maxLength, &maxLength, &errorLog[0]);

        for (int i = 0; i < errorLog.size(); i++) {
            std::cout << errorLog[i];
        }

        glDeleteShader(uid);
        return false;
    }

    return true;
}

Shader* shader_create()
{
    Shader* shader = new Shader;

    shader->Program = 0;
    shader->Shaders[0] = GL_NONE;
    shader->Shaders[1] = GL_NONE;
    shader->Shaders[2] = GL_NONE;

    return shader;
}

void shader_cleanup(Shader* shader)
{
    for (int i = 0; i < 3; i++) {
        if (shader->Shaders[i] == GL_NONE)
            continue;
        glDeleteShader(shader->Shaders[i]);
    }

    glDeleteProgram(shader->Program);
}

void shader_load(Shader* shader, std::filesystem::path path)
{

    assert(std::filesystem::exists(path));

    std::string loadedShaderSource = textFromFile(path);

    for (int i = 0; i < loadedShaderSource.length(); i++) {
        const char& c = loadedShaderSource[i];
        if (c == '#') {
            ShaderPreprocessorDefinition def = { .start = i };
            int j;
            for (j = ++i; loadedShaderSource[j] != ' '; j++) {
                def.key += loadedShaderSource[j];
            }
            for (j++; loadedShaderSource[j] != '\n'; j++) {
                def.def += loadedShaderSource[j];
            }
            def.end = j;
            i = j; // advance i
            def.def = trim(def.def);
            def.key = trim(def.key);
            shader->PreprocessorDefinitions.push_back(def);
        }
    }

    // now we have all of the key/value definitions
    // we can extract the relavent ones, for example
    // "type"
    std::vector<const ShaderPreprocessorDefinition*> types = getKeys(shader, "type");
    int i = 0;
    for (const ShaderPreprocessorDefinition* type : types) {
        GLuint glType = GL_NONE;
        if (type->def == "vertex")
            glType = GL_VERTEX_SHADER;
        if (type->def == "geometry")
            glType = GL_GEOMETRY_SHADER;
        if (type->def == "fragment")
            glType = GL_FRAGMENT_SHADER;

        assert(glType != GL_NONE);

        shader->Shaders[shader2Index[glType]] = glCreateShader(glType);

        const char* source = loadedShaderSource.c_str() + type->end;
        int end = types.size() - 1 == i ? types.size() : types[i + 1]->start;
        int length = end - type->end;

        glShaderSource(shader->Shaders[shader2Index[glType]], 1, &source, &length);
        i++;
    }
}

void shader_link(Shader* shader)
{
    shader->Program = glCreateProgram();

    for (int i = 0; i < 3; i++) {
        if (shader->Shaders[i] == GL_NONE)
            continue;

        glCompileShader(shader->Shaders[i]);
        if (!checkShader(shader->Shaders[i]))
            continue;

        glAttachShader(shader->Program, shader->Shaders[i]);
    }

    glLinkProgram(shader->Program);
}

void shader_add_attribute(Shader* shader, const std::string& attribute)
{
    shader->Attributes[attribute] = glGetAttribLocation(shader->Program, attribute.c_str());
}

void shader_add_uniform(Shader* shader, const std::string& uniform)
{
    shader->Uniforms[uniform] = glGetUniformLocation(shader->Program, uniform.c_str());
}

GLuint shader_get_attribute(Shader* shader, const std::string& attribute)
{
    return shader->Attributes[attribute];
}

GLuint shader_get_uniform(Shader* shader, const std::string& uniform)
{
    return shader->Uniforms[uniform];
}

void shader_use(Shader* shader)
{
    glUseProgram(shader->Program);
}

void shader_unuse(Shader* shader)
{
    glUseProgram(0);
}

}
