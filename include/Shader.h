// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>
#include <utility>

#include "Texture.h"

class Engine;

class Shader {
public:
	enum class Model {
		UNLIT,
		PHONG
	};

	class Uniform {
	public:
        static constexpr auto MATERIAL_AMBIENT   = "material.ambient";
        static constexpr auto MATERIAL_DIFFUSE   = "material.diffuse";
        static constexpr auto MATERIAL_SPECULAR  = "material.specular";
        static constexpr auto MATERIAL_SHININESS = "material.shininess";

        static constexpr auto TEXTURED_MATERIAL_DIFFUSE   = "texturedMaterial.diffuse";
        static constexpr auto TEXTURED_MATERIAL_SPECULAR  = "texturedMaterial.specular";
        static constexpr auto TEXTURED_MATERIAL_SHININESS = "texturedMaterial.shininess";

    private:
		static constexpr auto MODEL      = "model";
		static constexpr auto VIEW       = "view";
		static constexpr auto PROJECTION = "projection";
		static constexpr auto NORMAL_MAT = "normalMat";

		static constexpr auto DIRECTIONAL_LIGHT_DIRECTION = "directionalLight.direction";
		static constexpr auto DIRECTIONAL_LIGHT_AMBIENT   = "directionalLight.ambient";
		static constexpr auto DIRECTIONAL_LIGHT_DIFFUSE   = "directionalLight.diffuse";
		static constexpr auto DIRECTIONAL_LIGHT_SPECULAR  = "directionalLight.specular";

		static constexpr auto POINT_LIGHT_POSITION  = "pointLight.position";
		static constexpr auto POINT_LIGHT_AMBIENT   = "pointLight.ambient";
		static constexpr auto POINT_LIGHT_DIFFUSE   = "pointLight.diffuse";
		static constexpr auto POINT_LIGHT_SPECULAR  = "pointLight.specular";
		static constexpr auto POINT_LIGHT_CONSTANT  = "pointLight.constant";
		static constexpr auto POINT_LIGHT_LINEAR    = "pointLight.linear";
		static constexpr auto POINT_LIGHT_QUADRATIC = "pointLight.quadratic";

		static constexpr auto ENABLED_DIRECTIONAL_LIGHT = "enabledDirectionalLight";
		static constexpr auto ENABLED_POINT_LIGHT       = "enabledPointLight";
        static constexpr auto ENABLED_TEXTURED_MATERIAL = "enabledTexturedMaterial";

        friend class Renderer;
	};

	class Builder {
	public:
		explicit Builder(const Model model) : _model{ model } {}

		Shader* build(Engine& engine) const;

	private:
		const Model _model;

		[[nodiscard]] std::pair<std::string, std::string> resolveShaderUri() const;

		static GLuint createProgram(
			std::string_view vertexShaderUri,
			std::string_view fragmentShaderUri
		);

		static std::vector<char> readFile(std::string_view uri);

		static void validateCompilation(GLuint shader);
	};

	[[nodiscard]] GLuint getProgram() const;

	[[nodiscard]] Model getModel() const;

    [[nodiscard]] std::vector<std::pair<GLenum, GLuint>> getTextureBindings() const;

	void use() const;

	void setUniform(std::string_view name, bool value) const;
	void setUniform(std::string_view name, const float* matrix) const;
	void setUniform(std::string_view name, float value) const;
	void setUniform(std::string_view name, float x, float y, float z) const;
    void setUniform(std::string_view name, const Texture& texture);

private:
	explicit Shader(const GLuint program, const Model model)
	: _program{ program }, _model{ model } {}

	const GLuint _program;

	const Model _model;

    std::vector<std::pair<GLenum, GLuint>> _textureBindings{};
};