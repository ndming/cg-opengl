// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>

#include "Renderer.h"

#include "LightManager.h"
#include "RenderableManager.h"
#include "TransformManager.h"
#include "View.h"

void Renderer::render(const View& view) const {
    preprocessRendering(view);

	const auto scene = view.getScene();
	const auto camera = view.getCamera();

	if (!scene) {
		std::cerr << "Renderer: No scene was set for the view.\n";
		throw std::logic_error("Renderer: No scene was set for the view.\n");
	}

	if (!camera) {
		std::cerr << "Renderer: No camera was set for the view.\n";
		throw std::logic_error("Renderer: No camera was set for the view.\n");
	}

	// Render all renderables
	for (const auto entity : scene->_renderables) {
		// Compute MVP matrices
		const auto tm = TransformManager::getInstance();
		auto modelMat = glm::mat4(1.0f);
		if (tm->_transforms.contains(entity)) {
			modelMat = tm->_transforms[entity];
		}
		const auto viewMat = camera->getViewMatrix();
		const auto projMat = camera->getProjection();
		// Compute the normal matrix to save computation resource on the GPU
		const auto normalMat = glm::transpose(glm::inverse(viewMat * modelMat));

		const auto renderableManager = RenderableManager::getInstance();
		const auto& mesh = renderableManager->_meshes[entity];

		// For each geometry in this renderable
		for (std::size_t i = 0; i < mesh->elements.size(); ++i) {
            // Each mesh has a corresponding shader
			const auto shader = mesh->shaders[i];
            // Specify which program to use first
			shader->use();

			// Set the MVP and the normal matrix
			shader->setUniform(Shader::Uniform::MODEL, value_ptr(modelMat));
			shader->setUniform(Shader::Uniform::VIEW, value_ptr(viewMat));
			shader->setUniform(Shader::Uniform::PROJECTION, value_ptr(projMat));
			shader->setUniform(Shader::Uniform::NORMAL_MAT, value_ptr(normalMat));

			// Disable all lights in case no light is set for this scene
			shader->setUniform(Shader::Uniform::ENABLED_DIRECTIONAL_LIGHT, false);
            shader->setUniform(Shader::Uniform::POINT_LIGHT_COUNT, 0);

			// Render all lights for this geometry
			const auto lightManager = LightManager::getInstance();
            auto pointLightCount = 0;
			for (const auto light : scene->_lights) {
				if (lightManager->_directionalLights.contains(light)) {
                    renderDirectionalLight(light, shader, viewMat);
				}
				else if (
                    lightManager->_pointLights.contains(light) &&
                    pointLightCount < Shader::Uniform::MAX_POINT_LIGHT_COUNT
                ) {
                    renderPointLight(light, shader, viewMat, pointLightCount++);
				}
			}
            shader->setUniform(Shader::Uniform::POINT_LIGHT_COUNT, pointLightCount);

			// Draw using index buffer
			const auto& element = mesh->elements[i];

			// VAO will be linked to the currently used program
			glBindVertexArray(element->vao);

			// Enable texture bindings if there are textures set for this shader
            const auto textureBindings = shader->getTextureBindings();
            switch (shader->getModel()) {
                case Shader::Model::UNLIT:
                    shader->setUniform(Shader::Uniform::ENABLED_UNLIT_TEXTURE, !textureBindings.empty());
                    break;
                case Shader::Model::PHONG:
                    shader->setUniform(Shader::Uniform::ENABLED_TEXTURED_MATERIAL, !textureBindings.empty());
                    break;
            }
            // Bind the textures before the draw call
            for (auto tex = 0; tex < static_cast<int>(textureBindings.size()); ++tex) {
                glActiveTexture(GL_TEXTURE0 + tex);
                const auto& [target, texture] = textureBindings[tex];
                glBindTexture(target, texture);
            }
			
			glDrawElements(
				element->topology, static_cast<GLsizei>(element->count), element->indexType,
				reinterpret_cast<void*>(static_cast<uint64_t>(element->offset)) // NOLINT(performance-no-int-to-ptr)
			);

			glBindVertexArray(0);
		}
	}
}

void Renderer::preprocessRendering(const View& view) const {
    const auto vp = view.getViewport();
    glViewport(vp[0], vp[1], vp[2], vp[3]);

    auto clearMask = GL_DEPTH_BUFFER_BIT;
    if (_clearOptions.clear) {
        glClearColor(
            _clearOptions.clearColor[0],
            _clearOptions.clearColor[1],
            _clearOptions.clearColor[2],
            _clearOptions.clearColor[3]
        );
        clearMask |= GL_COLOR_BUFFER_BIT;
    }
    glClear(clearMask);
}

void Renderer::renderDirectionalLight(Entity light, Shader* const shader, const glm::mat4& viewMat) {
    const auto& dirLight = LightManager::getInstance()->_directionalLights[light];
    const auto lightNormalMat = glm::transpose(glm::inverse(viewMat * glm::mat4(1.0f)));
    const auto direction = glm::normalize(glm::vec3(lightNormalMat * glm::vec4(dirLight->direction, 0.0f)));

    shader->setUniform(Shader::Uniform::DIRECTIONAL_LIGHT_DIRECTION, direction.x, direction.y, direction.z);
    shader->setUniform(Shader::Uniform::DIRECTIONAL_LIGHT_AMBIENT, dirLight->ambient.x, dirLight->ambient.y, dirLight->ambient.z);
    shader->setUniform(Shader::Uniform::DIRECTIONAL_LIGHT_DIFFUSE, dirLight->diffuse.x, dirLight->diffuse.y, dirLight->diffuse.z);
    shader->setUniform(Shader::Uniform::DIRECTIONAL_LIGHT_SPECULAR, dirLight->specular.x, dirLight->specular.y, dirLight->specular.z);

    // Enable directional light
    shader->setUniform(Shader::Uniform::ENABLED_DIRECTIONAL_LIGHT, true);
}

void Renderer::renderPointLight(Entity light, Shader* const shader, const glm::mat4 &viewMat, const int idx) {
    const auto& pointLight = LightManager::getInstance()->_pointLights[light];

    // The position of point light in camera space
    const auto lightPos = viewMat * glm::vec4{ pointLight->position, 1.0f };
    const auto plPos = Shader::Uniform::pointLightPositionAt(idx);
    shader->setUniform(plPos, lightPos.x / lightPos.w, lightPos.y / lightPos.w, lightPos.z / lightPos.w);

    const auto plAmbient = Shader::Uniform::pointLightAmbientAt(idx);
    const auto plDiffuse = Shader::Uniform::pointLightDiffuseAt(idx);
    const auto plSpecular = Shader::Uniform::pointLightSpecularAt(idx);
    shader->setUniform(plAmbient, pointLight->ambient.x, pointLight->ambient.y, pointLight->ambient.z);
    shader->setUniform(plDiffuse, pointLight->diffuse.x, pointLight->diffuse.y, pointLight->diffuse.z);
    shader->setUniform(plSpecular, pointLight->specular.x, pointLight->specular.y, pointLight->specular.z);

    const auto plConstant = Shader::Uniform::pointLightConstantAt(idx);
    const auto plLinear = Shader::Uniform::pointLightLinearAt(idx);
    const auto plQuadratic = Shader::Uniform::pointLightQuadraticAt(idx);
    shader->setUniform(plConstant, pointLight->constant);
    shader->setUniform(plLinear, pointLight->linear);
    shader->setUniform(plQuadratic, pointLight->quadratic);
}

void Renderer::setClearOptions(const ClearOptions& options) {
	_clearOptions = options;
}

Renderer::ClearOptions Renderer::getClearOptions() const {
	return _clearOptions;
}

void Renderer::togglePolygonMode() {
	if (_polygonMode == PolygonMode::FILL) {
		_polygonMode = PolygonMode::LINE;
		glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(_polygonMode));
	} else {
		_polygonMode = PolygonMode::FILL;
		glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(_polygonMode));
	}
}

void Renderer::readFramebufferRgba(const int x, const int y, const int width, const int height, unsigned char *data) {
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
}
