// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <glad/glad.h>
#include <ranges>
#include <memory>

#include "Engine.h"

std::unique_ptr<Engine> Engine::create() {
    return std::unique_ptr<Engine>(new Engine{});
}

Engine::Engine() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
}

EntityManager* Engine::getEntityManager() const {
	return _entityManager;
}

RenderableManager* Engine::getRenderableManager() const {
	return _renderableManager;
}

LightManager* Engine::getLightManager() const {
	return _lightManager;
}

TransformManager* Engine::getTransformManager() const {
	return _transformManager;
}

Renderer* Engine::createRenderer() {
	const auto renderer = new Renderer();
	_renderers.insert(renderer);
	return renderer;
}

void Engine::destroyRenderer(Renderer* const renderer) {
	if (renderer) {
		_renderers.erase(renderer);
		delete renderer;
	}
}

View* Engine::createView() {
	const auto view = new View();
	_views.insert(view);
	return view;
}

void Engine::destroyView(View* const view) {
	if (view) {
		_views.erase(view);
		delete view;
	}
}

void Engine::destroySkybox(Skybox* const skybox) {
    if (skybox) {
        _skyboxes.erase(skybox);
        delete skybox;
    }
}

Scene* Engine::createScene() {
	const auto scene = new Scene();
	_scenes.insert(scene);
	return scene;
}

void Engine::destroyScene(Scene* const scene) {
	if (scene) {
		_scenes.erase(scene);
		delete scene;
	}
}

void Engine::destroyVertexBuffer(VertexBuffer* const buffer) {
	if (buffer) {
		_vertexBuffers.erase(buffer);
		glDeleteBuffers(buffer->getBufferCount(), buffer->_bufferObjects);
		delete[] buffer->_bufferObjects;
		delete buffer;
	}
}

void Engine::destroyIndexBuffer(IndexBuffer* const buffer) {
	if (buffer) {
		_indexBuffers.erase(buffer);
		const auto ibo = buffer->getNativeObject();
		glDeleteBuffers(1, &ibo);
		delete buffer;
	}
}

void Engine::destroyShader(Shader* const shader) {
	if (shader) {
		_shaders.erase(shader);
		glDeleteProgram(shader->getProgram());
		delete shader;
	}
}

Camera* Engine::createCamera(const Entity entity) {
	const auto camera = new Camera(entity);
	_cameras[entity] = camera;
	return camera;
}

void Engine::destroyCamera(const Entity entity) {
	if (_cameras.contains(entity)) {
		delete _cameras[entity];
		_cameras.erase(entity);
	}
}

void Engine::destroyEntity(const Entity entity) const {
	if (_renderableManager->hasComponent(entity)) {
		for (const auto& element : _renderableManager->_meshes[entity]->elements) {
			glDeleteVertexArrays(1, &element->vao);
		}
		_renderableManager->_meshes.erase(entity);

		// Remove the associated component of this entity
		_entityManager->_entities[entity] = EntityManager::Component::NONE;
		// ...and the associated transform also.
		_transformManager->_transforms.erase(entity);
	} else if (_lightManager->hasComponent(entity)) {
		_lightManager->_directionalLights.erase(entity);
		_lightManager->_pointLights.erase(entity);
		
		_entityManager->_entities[entity] = EntityManager::Component::NONE;
		_transformManager->_transforms.erase(entity);
	}
}

void Engine::destroyTexture(Texture* const texture) {
    if (texture) {
        _textures.erase(texture);
        const auto id = texture->getNativeObject();
        glDeleteTextures(1, &id);
        delete texture;
    }
}

void Engine::destroy() {
	// Destroy any remaining renderable entities
	for (const auto& mesh : _renderableManager->_meshes | std::views::values) {
		for (const auto& element : mesh->elements) {
			glDeleteVertexArrays(1, &element->vao);
		}
	}
	_renderableManager->_meshes.clear();

	// Destroy any remaining light entities
	_lightManager->_directionalLights.clear();
	_lightManager->_pointLights.clear();

    // Destroy any remaining textures
    for (const auto texture : _textures) {
        const auto id = texture->getNativeObject();
        glDeleteTextures(1, &id);
        delete texture;
    }
    _textures.clear();

	// Destroy any remaining renderer
	for (const auto renderer : _renderers) {
		delete renderer;
	}
	_renderers.clear();

	// Destroy any remaining view
	for (const auto view : _views) {
		delete view;
	}
	_views.clear();

    // Destroy any remaining skyboxes
    for (const auto skybox : _skyboxes) {
        delete skybox;
    }
    _skyboxes.clear();

	// Destroy any remaining vertex buffers
	for (const auto& buffer : _vertexBuffers) {
		glDeleteBuffers(buffer->getBufferCount(), buffer->_bufferObjects);
		delete[] buffer->_bufferObjects;
		delete buffer;
	}
	_vertexBuffers.clear();

	// Destroy any remaining index buffers
	for (const auto& buffer : _indexBuffers) {
		const auto ibo = buffer->getNativeObject();
		glDeleteBuffers(1, &ibo);
		delete buffer;
	}
	_indexBuffers.clear();

	// Destroy any remaining shaders
	for (const auto shader : _shaders) {
		glDeleteProgram(shader->getProgram());
		delete shader;
	}
	_shaders.clear();

	// Destroy any remaining camera resources
	for (const auto ptr : _cameras | std::views::values) {
		delete ptr;
	}
	_cameras.clear();

	// Destroy the entity manager
	delete _entityManager;

	// Destroy the renderable manager
	delete _renderableManager;

	// Destroy the light manager
	delete _lightManager;

	// Destroy the transform manager
	delete _transformManager;
}