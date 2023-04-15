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
		const auto tcm = TransformManager::getInstance();
		auto modelMat = glm::mat4(1.0f);
		if (tcm->_transforms.contains(entity)) {
			modelMat = tcm->_transforms[entity];
		}
		const auto viewMat = camera->getViewMatrix();
		const auto projMat = camera->getProjection();
		// Compute the normal matrix to save computation resource on the GPU
		const auto normalMat = transpose(inverse(viewMat * modelMat));

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
			shader->setUniform(Shader::Uniform::ENABLED_POINT_LIGHT, false);

			// Render all lights for this geometry
			const auto lightManager = LightManager::getInstance();
			for (const auto light : scene->_lights) {
				if (lightManager->_directionalLights.contains(light)) {
					const auto& dirLight = lightManager->_directionalLights[light];
					shader->setUniform(
                        Shader::Uniform::DIRECTIONAL_LIGHT_DIRECTION,
                        dirLight->direction.x, dirLight->direction.y, dirLight->direction.z
                    );
					shader->setUniform(
                        Shader::Uniform::DIRECTIONAL_LIGHT_AMBIENT,
                        dirLight->ambient.x, dirLight->ambient.y, dirLight->ambient.z
					);
					shader->setUniform(
						Shader::Uniform::DIRECTIONAL_LIGHT_DIFFUSE,
						dirLight->diffuse.x, dirLight->diffuse.y, dirLight->diffuse.z
					);
					shader->setUniform(
						Shader::Uniform::DIRECTIONAL_LIGHT_SPECULAR,
						dirLight->specular.x, dirLight->specular.y, dirLight->specular.z
					);
					// Enable directional light
					shader->setUniform(Shader::Uniform::ENABLED_DIRECTIONAL_LIGHT, true);
				}
				else if (lightManager->_pointLights.contains(light)) {
					const auto& pointLight = lightManager->_pointLights[light];

					// Point light may have an associated transform component.
					// auto lightModelMat = glm::mat4(1.0f);
					// if (tcm->_transforms.contains(light)) {
					// 	lightModelMat = tcm->_transforms[light];
					// }
					// The position of point light in camera space
					const auto lightPos = viewMat * glm::vec4{ pointLight->position, 1.0f };

					shader->setUniform(
						Shader::Uniform::POINT_LIGHT_POSITION,
						lightPos.x / lightPos.w, lightPos.y / lightPos.w, lightPos.z / lightPos.w
					);
					shader->setUniform(
						Shader::Uniform::POINT_LIGHT_AMBIENT,
						pointLight->ambient.x, pointLight->ambient.y, pointLight->ambient.z
					);
					shader->setUniform(
						Shader::Uniform::POINT_LIGHT_DIFFUSE,
						pointLight->diffuse.x, pointLight->diffuse.y, pointLight->diffuse.z
					);
					shader->setUniform(
						Shader::Uniform::POINT_LIGHT_SPECULAR,
						pointLight->specular.x, pointLight->specular.y, pointLight->specular.z
					);
					shader->setUniform(Shader::Uniform::POINT_LIGHT_CONSTANT, pointLight->constant);
					shader->setUniform(Shader::Uniform::POINT_LIGHT_LINEAR, pointLight->linear);
					shader->setUniform(Shader::Uniform::POINT_LIGHT_QUADRATIC, pointLight->quadratic);
					// Enable point light
					shader->setUniform(Shader::Uniform::ENABLED_POINT_LIGHT, true);
				}
			}

			// Draw using index buffer
			const auto& element = mesh->elements[i];

			// VAO will be linked to the currently used program
			glBindVertexArray(element->vao);

			// Bind the texture before the draw call
			// glActiveTexture(GL_TEXTURE0 + i);
			// glBindTexture(GL_TEXTURE_2D, texture);
			
			glDrawElements(
				element->topology, static_cast<GLsizei>(element->count), element->indexType,
				reinterpret_cast<void*>(static_cast<uint64_t>(element->offset)) // NOLINT(performance-no-int-to-ptr)
			);

			glBindVertexArray(0);
		}
	}
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
