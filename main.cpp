// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Context.h"
#include "Engine.h"
#include "EntityManager.h"
#include "Renderer.h"
#include "Shader.h"

#include "drawable/Cone.h"
#include "drawable/Cube.h"
#include "drawable/Cylinder.h"
#include "drawable/Frustum.h"
#include "drawable/Mesh.h"
#include "drawable/Sphere.h"
#include "drawable/Pyramid.h"
#include "drawable/Tetrahedron.h"

#include "utils/MediaExporter.h"
#include "utils/TextureLoader.h"

int main() {
    // The window context
    auto context = Context::create("1952092");
    context->setOnPress(Context::Key::ESC, [&context] {
        context->setClose(true);
    });

    // The rendering engine
    const auto engine = Engine::create();

    // Create a renderer
    const auto renderer = engine->createRenderer();

    // Toggle polygon mode on T press
    context->setOnPress(Context::Key::T, [&renderer] {
        renderer->togglePolygonMode();
    });

    // Capture the framebuffer on F1 press
    const auto exporter = MediaExporter::Builder()
            .folderPath("capture")
            .build();
    context->setOnPress(Context::Key::F1, [&context, &exporter] {
        // Acquire the framebuffer size
        const auto& [w, h] = context->getFramebufferSize();
        // Prepare a placeholder for RGBA image
        const auto data = new unsigned char[w * h * 4];
        // Load the content of the framebuffer
        Renderer::readFramebufferRgba(0, 0, w, h, data);
        // OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom side of the image,
        // but images usually have 0.0 at the top of the y-axis.
        for (int y = 0; y < h / 2; y++) {
            int i1 = y * w * 4;
            int i2 = (h - y - 1) * w * 4;
            for (int x = 0; x < w * 4; x++) {
                std::swap(data[i1 + x], data[i2 + x]);
            }
        }
        exporter->exportImage("scene", data, w, h);
        delete[] data;
    });

    // Create a camera
    const auto camera = engine->createCamera(EntityManager::get()->create());
    context->setMouseScrollCallback([&camera](const auto offsetY) {
        camera->relativeZoom(offsetY);
    });
    context->setMouseDragPerpetualCallback([&camera](const auto offsetX, const auto offsetY) {
        camera->relativeDrag(offsetX, offsetY);
    });

    // Create a scene
    const auto scene = engine->createScene();

    // Create and set up a view
    const auto view = engine->createView();
    // Create a skybox
    const auto skybox = Skybox::Builder().color(0.09804f, 0.14118f, 0.15686f, 1.0f).build(*engine);
    view->setSkybox(skybox);
    view->setCamera(camera);
    view->setScene(scene);

    // Update the projection and viewport on window resize
    context->setFramebufferCallback([&](const auto w, const auto h) {
        const auto ratio = static_cast<float>(w) / static_cast<float>(h);
        camera->setProjection(45.0f, ratio, 0.1f, 100.0f);
        view->setViewport({ 0, 0, w, h });
    });

    const auto tcm = engine->getTransformManager();


    const auto brickTexture = loadTexture("brick/brick_diffuse.png", *engine);
    const auto cube = Cube::Builder()
            .textureUnlit(brickTexture)
            .shaderModel(Shader::Model::UNLIT)
            .build(*engine);
    const auto cubeTrans = translate(glm::mat4(1.0f), glm::vec3{ 3.0f, 3.0f, 3.0f });
    tcm->setTransform(cube->getEntity(), cubeTrans);

    const auto frustum = Frustum::Builder()
            .build(*engine);
    const auto frustumTrans = translate(glm::mat4(1.0f), glm::vec3{ 3.0f, 3.0f, -3.0f });
    tcm->setTransform(frustum->getEntity(), frustumTrans);

    const auto sunTexture = loadTexture("sun/sun_diffuse.jpg", *engine);
    const auto tetra = Tetrahedron::Builder()
            .shaderModel(Shader::Model::UNLIT)
            .textureUnlit(sunTexture)
            .build(*engine);
    const auto tetraTrans = translate(glm::mat4(1.0f), glm::vec3{ -3.0f, 3.0f, 3.0f });
    tcm->setTransform(tetra->getEntity(), tetraTrans);

    const auto earthTexture = loadTexture("earth/earth_diffuse.png", *engine);
    const auto geoSphere = Sphere::GeographicBuilder()
            .textureUnlit(earthTexture)
            .shaderModel(Shader::Model::UNLIT)
            .build(*engine);
    const auto geoSphereTrans = translate(glm::mat4(1.0f), glm::vec3{ 3.0f, -3.0f, 3.0f });
    tcm->setTransform(geoSphere->getEntity(), geoSphereTrans);

    const auto divSphere = Sphere::SubdivisionBuilder()
            .shaderModel(Shader::Model::UNLIT)
            .build(*engine);
    const auto divSphereTrans = translate(glm::mat4(1.0f), glm::vec3{ -3.0f, -3.0f, 3.0f });
    tcm->setTransform(divSphere->getEntity(), divSphereTrans);

    const auto cylinder = Cylinder::Builder().build(*engine);
    const auto cylinderTrans = translate(glm::mat4(1.0f), glm::vec3{ 3.0f, -3.0f, -3.0f });
    tcm->setTransform(cylinder->getEntity(), cylinderTrans);

    const auto cone = Cone::Builder().build(*engine);
    const auto coneTrans = translate(glm::mat4(1.0f), glm::vec3{ -3.0f, -3.0f, -3.0f });
    tcm->setTransform(cone->getEntity(), coneTrans);

    const auto pyramid = Pyramid::Builder().build(*engine);
    const auto pyramidTrans = translate(glm::mat4(1.0f), glm::vec3{ -3.0f, 3.0f, -3.0f });
    tcm->setTransform(pyramid->getEntity(), pyramidTrans);

    const auto func = std::function{ [](const float x, const float y) {
        return glm::sin(x) / (x * x + 1) + glm::sin(y) / (y * y + 1);
    }};

    const auto moonTexture = loadTexture("moon/moon_diffuse.jpg", *engine);
    const auto mesh = Mesh::Builder(func)
            .halfExtentX(4.0f)
            .halfExtentY(4.0f)
            .segments(100)
            .textureUnlit(moonTexture)
            .shaderModel(Shader::Model::UNLIT)
            .build(*engine);

    scene->addEntity(cube->getEntity());
    // scene->addEntity(frustum->getEntity());
    scene->addEntity(tetra->getEntity());
    scene->addEntity(geoSphere->getEntity());
    // scene->addEntity(divSphere->getEntity());
    // scene->addEntity(cylinder->getEntity());
    // scene->addEntity(cone->getEntity());
    // scene->addEntity(pyramid->getEntity());
    scene->addEntity(mesh->getEntity());

    // The render loop
    context->loop([&] { renderer->render(*view); });

    // Free up any resources we may have forgotten to destroy
    engine->destroy();

    return 0;
}