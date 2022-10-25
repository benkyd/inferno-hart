#pragma once

#include "../graphics.hpp"

#include <string>
#include <memory>
#include <vector>
#include <filesystem>
#include <unordered_map>

namespace inferno {

class _Shader : public std::enable_shared_from_this<_Shader> {
public:
	_Shader();
	~_Shader();

	std::shared_ptr<_Shader> preprocessorDefine(std::string statement);

	std::shared_ptr<_Shader> load(std::filesystem::path path);
	std::shared_ptr<_Shader> link();

	std::shared_ptr<_Shader> use();
	std::shared_ptr<_Shader> unUse();

	void addAttribute(const std::string& attribute);
	void addUniform(const std::string& uniform);
	GLuint operator[](const std::string& attribute) {
		return mAttributes[attribute];
	}
	GLuint operator()(const std::string& uniform) {
		return mUniformLocations[uniform];
	}

private:
	GLuint mShaders[3];
	GLuint mProgram;

	std::unordered_map<std::string, GLuint> mAttributes;
	std::unordered_map<std::string, GLuint> mUniformLocations;
	bool mCheckShader(GLuint uid);

private:
	// preprocessor definitions can be defined by their
	// start/end index, this is so that text can either be inserted
	// in their place or for the "type" definition, the program
	// can count until the next "type" definition per shader
	struct mPreprocessorDefinition {
		int start, end;
		std::string key;
		std::string def;
	};
	std::vector<mPreprocessorDefinition> mDefinitions;
	std::vector<const _Shader::mPreprocessorDefinition*> mGetKeys(std::string key);
};
typedef std::shared_ptr<_Shader> Shader;

}
