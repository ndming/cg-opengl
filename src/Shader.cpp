// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>

#include "Engine.h"
#include "Shader.h"

std::pair<std::string, std::string> Shader::Builder::resolveShaderUri() const {
	switch (_model) {
	case Model::UNLIT: 
		return std::make_pair("./res/shaders/unlit.vert", "./res/shaders/unlit.frag");
	case Model::PHONG: 
		return std::make_pair("./res/shaders/phong.vert", "./res/shaders/phong.frag");
	}
	return {};
}

Shader* Shader::Builder::build(Engine& engine) const {
	const auto [vertShader, fragShader] = resolveShaderUri();
	if (vertShader.empty() || fragShader.empty()) {
		throw std::runtime_error("SHADER: Could not resolve shader paths.\n");
	}

	const auto program = createProgram(vertShader, fragShader);
	const auto shader = new Shader(program, _model);

	engine._shaders.insert(shader);

	return shader;
}

GLuint Shader::Builder::createProgram(
	const std::string_view vertexShaderUri, 
	const std::string_view fragmentShaderUri
) {
	const auto vertexShaderCode = readFile(vertexShaderUri);
	const auto vertexShaderSource = vertexShaderCode.data();
	const auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	validateCompilation(vertexShader);

	const auto fragmentShaderCode = readFile(fragmentShaderUri);
	const auto fragmentShaderSource = fragmentShaderCode.data();
	const auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	validateCompilation(fragmentShader);

	const auto shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cout << "SHADER: Linking Failed\n" << infoLog << '\n';
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

std::vector<char> Shader::Builder::readFile(const std::string_view uri) {
	auto file = std::ifstream(uri.data(), std::ios::ate);
	if (!file.is_open()) {
		throw std::runtime_error("SHADER: Failed to open file!");
	}
	const auto fileSize = static_cast<size_t>(file.tellg());
	auto buffer = std::vector<char>(fileSize);
	file.seekg(0);
	file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
	file.close();
	return buffer;
}

void Shader::Builder::validateCompilation(const GLuint shader) {
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << "SHADER: Compilation Failed:\n" << infoLog << '\n';
	}
}

GLuint Shader::getProgram() const {
	return _program;
}

Shader::Model Shader::getModel() const {
	return _model;
}

std::vector<std::pair<GLenum, GLuint>> Shader::getTextureBindings() const {
    return _textureBindings;
}

void Shader::use() const {
	glUseProgram(_program);
}

void Shader::setUniform(const std::string_view name, const bool value) const {
	const auto location = glGetUniformLocation(_program, name.data());
	glUniform1i(location, value);
}

void Shader::setUniform(const std::string_view name, const float* const matrix) const {
	const auto location = glGetUniformLocation(_program, name.data());
	glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}

void Shader::setUniform(const std::string_view name, const float value) const {
	const auto location = glGetUniformLocation(_program, name.data());
	glUniform1f(location, value);
}

void Shader::setUniform(const std::string_view name, const float x, const float y, const float z) const {
	const auto location = glGetUniformLocation(_program, name.data());
	glUniform3f(location, x, y, z);
}

void Shader::setUniform(const std::string_view name, const Texture &texture) {
    const auto location = glGetUniformLocation(_program, name.data());
    const auto texUnit = static_cast<int>(_textureBindings.size());
    glUniform1i(location, texUnit);
    _textureBindings.emplace_back(texture.getTarget(), texture.getNativeObject());
}