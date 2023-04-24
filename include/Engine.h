// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <memory>
#include <set>
#include <unordered_map>

#include "Camera.h"
#include "EntityManager.h"
#include "IndexBuffer.h"
#include "LightManager.h"
#include "RenderableManager.h"
#include "Renderer.h"
#include "Scene.h"
#include "Skybox.h"
#include "Shader.h"
#include "Texture.h"
#include "TransformManager.h"
#include "VertexBuffer.h"
#include "View.h"

class Engine {
public:
	~Engine() = default;
	Engine(const Engine&) = delete;
	Engine(Engine&&) noexcept = delete;
	Engine& operator=(const Engine&) = delete;
	Engine& operator=(Engine&&) noexcept = delete;

	static std::unique_ptr<Engine> create();

	[[nodiscard]] EntityManager* getEntityManager() const;

	[[nodiscard]] RenderableManager* getRenderableManager() const;

	[[nodiscard]] LightManager* getLightManager() const;

	[[nodiscard]] TransformManager* getTransformManager() const;

	[[nodiscard]] Renderer* createRenderer();

	void destroyRenderer(Renderer* renderer);

	[[nodiscard]] View* createView();

	void destroyView(View* view);

    void destroySkybox(Skybox* skybox);

	[[nodiscard]] Scene* createScene();

	void destroyScene(Scene* scene);

	void destroyVertexBuffer(VertexBuffer* buffer);

	void destroyIndexBuffer(IndexBuffer* buffer);

	void destroyShader(Shader* shader);

	[[nodiscard]] Camera* createCamera(Entity entity);

	void destroyCamera(Entity entity);

	void destroyEntity(Entity entity) const;

    void destroyTexture(Texture* texture);

	void destroy();

private:
	explicit Engine();

	EntityManager* const _entityManager{ EntityManager::get() };

	RenderableManager* const _renderableManager{ RenderableManager::getInstance() };

	LightManager* const _lightManager{ LightManager::getInstance() };

	TransformManager* const _transformManager{ TransformManager::getInstance() };

	std::set<Renderer*> _renderers{};

	std::set<View*> _views{};

    std::set<Skybox*> _skyboxes{};

	std::set<Scene*> _scenes{};

	std::unordered_map<Entity, Camera*> _cameras{};

	std::set<VertexBuffer*> _vertexBuffers{};

	std::set<IndexBuffer*> _indexBuffers{};

	std::set<Shader*> _shaders{};

    std::set<Texture*> _textures{};

	friend class IndexBuffer;
	friend class Shader;
    friend class Skybox;
    friend class Texture;
	friend class VertexBuffer;
};
