#include "shader.hpp"

#include <iostream>
#include <fstream>

inline std::string trim(std::string& str)
{
    str.erase(str.find_last_not_of(' ')+1);         //suffixing spaces
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
    return str;
}

static std::unordered_map<GLuint, int> Shader2Index = {
	{GL_VERTEX_SHADER, 0},
	{GL_GEOMETRY_SHADER, 1},
	{GL_FRAGMENT_SHADER, 2}
};

using namespace inferno;

std::string textFromFile(const std::filesystem::path& path) {
    std::ifstream input(path);
    return std::string((std::istreambuf_iterator<char>(input)), 
                        std::istreambuf_iterator<char>());

}

Shader::Shader()
	: mShaders({ GL_NONE, GL_NONE, GL_NONE})
	, mProgram(0) {
}


Shader::~Shader() {
	for (int i = 0; i < 3; i++) {
		if (mShaders[i] == GL_NONE) continue;
		glDeleteShader(mShaders[i]);
	}
}

Shader* Shader::load(std::filesystem::path path) {

	assert(std::filesystem::exists(path));

	std::string loadedShaderSource = textFromFile(path);

	for (int i = 0; i < loadedShaderSource.length(); i++) {
		const char& c = loadedShaderSource[i];
		if (c == '#') {
			mPreprocessorDefinition def = { .start = i };
			int j;
			for (j = ++i; loadedShaderSource[j] != ' '; j++) {
				def.key += loadedShaderSource[j];
			}
			for (j++; loadedShaderSource[j] != '\n'; j++) {
				def.def += loadedShaderSource[j];
			}
			def.end = j; i = j; // advance i
			def.def = trim(def.def);
			def.key = trim(def.key);
			mDefinitions.push_back(def);
		}
	}

	// now we have all of the key/value definitions
	// we can extract the relavent ones, for example
	// "type"
	auto types = mGetKeys("type");
	int i = 0;
	for (const mPreprocessorDefinition* type : types) {
		GLuint glType = GL_NONE;
		if (type->def == "vertex") glType = GL_VERTEX_SHADER;
		if (type->def == "geometry") glType = GL_GEOMETRY_SHADER;
		if (type->def == "fragment") glType = GL_FRAGMENT_SHADER;
		std::cout << "TYPE: " << type->def << " i " << glType << std::endl;

		assert(glType != GL_NONE);

		mShaders[Shader2Index[glType]] = glCreateShader(glType);
		
		const char* source = loadedShaderSource.c_str() + type->end;
		int end = types.size() - 1 == i ? types.size(): types[i + 1]->start;
		int length = end - type->end;
		
		glShaderSource(mShaders[Shader2Index[glType]], 1, &source, &length);
		i++;
	}

	return this;
}

Shader* Shader::link() {

	mProgram = glCreateProgram();
	
	for (int i = 0; i < 3; i++) {
		if (mShaders[i] == GL_NONE) continue;

		glCompileShader(mShaders[i]);
		if (!mCheckShader(mShaders[i])) continue;

		glAttachShader(mProgram, mShaders[i]);
	}

	glLinkProgram(mProgram);
	return this;
}

Shader* Shader::use() {
	glUseProgram(mProgram);
	return this;
}

Shader* Shader::unUse() {
	glUseProgram(0);
	return this;
}

GLuint Shader::getProgram()
{
	return mProgram;
}


void Shader::addAttribute(const std::string& attribute) {
	mAttributes[attribute] = glGetAttribLocation(mProgram, attribute.c_str());
}

void Shader::addUniform(const std::string& uniform) {
	mUniformLocations[uniform] = glGetUniformLocation(mProgram, uniform.c_str());
}

bool Shader::mCheckShader(GLuint uid) {
	GLint isCompiled = 0;
	glGetShaderiv(uid, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
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

std::vector<const Shader::mPreprocessorDefinition*> Shader::mGetKeys(std::string key) {
	std::vector<const Shader::mPreprocessorDefinition*> ret;
	for (const auto& p : mDefinitions) if (p.key == key) ret.push_back(&p);
	return ret;
}
