// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <glm/gtc/matrix_transform.hpp>

#include "Context.h"
#include "Engine.h"
#include "EntityManager.h"
#include "LightManager.h"
#include "Renderer.h"
#include "Shader.h"

#include "drawable/Aura.h"
#include "drawable/Cone.h"
#include "drawable/Cube.h"
#include "drawable/Cylinder.h"
#include "drawable/Frustum.h"
#include "drawable/Material.h"
#include "drawable/Mesh.h"
#include "drawable/Sphere.h"
#include "drawable/Pyramid.h"
#include "drawable/Tetrahedron.h"

#include "utils/MediaExporter.h"

inline float getAuraVelocity(long deltaTimeMillis, float speed);

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

    // Clear the color buffer
    auto clearOptions = renderer->getClearOptions();
    // clearOptions.clearColor = { 0.09804f, 0.14118f, 0.15686f, 1.0f };
    clearOptions.clearColor = { 0.0f, 0.0196f, 0.02157f, 1.0f };
    renderer->setClearOptions(clearOptions);

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
    camera->setRadius(8.0f);
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
    const auto skybox = Skybox::Builder().color(0.02f, 0.04f, 0.06f, 1.0f).build(*engine);
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

    const auto cube = Cube::Builder()
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::JADE)
            .build(*engine);
    const auto cubeTrans = translate(glm::mat4(1.0f), glm::vec3{ 3.0f, 3.0f, 3.0f });
    tcm->setTransform(cube->getEntity(), cubeTrans);

    const auto frustum = Frustum::Builder()
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::WHITE_PLASTIC)
            .build(*engine);
    const auto frustumTrans = translate(glm::mat4(1.0f), glm::vec3{ 3.0f, 3.0f, -3.0f });
    tcm->setTransform(frustum->getEntity(), frustumTrans);

    const auto tetra = Tetrahedron::Builder()
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::GOLD)
            .build(*engine);
    const auto tetraTrans = translate(glm::mat4(1.0f), glm::vec3{ -3.0f, 3.0f, 3.0f });
    tcm->setTransform(tetra->getEntity(), tetraTrans);

    const auto geoSphere = Sphere::GeographicBuilder()
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::RUBY)
            .build(*engine);
    const auto geoSphereTrans = translate(glm::mat4(1.0f), glm::vec3{ 3.0f, -3.0f, 3.0f });
    tcm->setTransform(geoSphere->getEntity(), geoSphereTrans);

    const auto divSphere = Sphere::SubdivisionBuilder()
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::PEARL)
            .build(*engine);
    const auto divSphereTrans = translate(glm::mat4(1.0f), glm::vec3{ -3.0f, -3.0f, 3.0f });
    tcm->setTransform(divSphere->getEntity(), divSphereTrans);

    const auto cylinder = Cylinder::Builder()
            .segments(80)
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::EMERALD)
            .build(*engine);
    const auto cylinderTrans = translate(glm::mat4(1.0f), glm::vec3{ 3.0f, -3.0f, -3.0f });
    tcm->setTransform(cylinder->getEntity(), cylinderTrans);

    const auto cone = Cone::Builder()
            .segments(80)
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::COPPER)
            .build(*engine);
    const auto coneTrans = translate(glm::mat4(1.0f), glm::vec3{ -3.0f, -3.0f, -3.0f });
    tcm->setTransform(cone->getEntity(), coneTrans);

    const auto pyramid = Pyramid::Builder()
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::CYAN_PLASTIC)
            .build(*engine);
    const auto pyramidTrans = translate(glm::mat4(1.0f), glm::vec3{ -3.0f, 3.0f, -3.0f });
    tcm->setTransform(pyramid->getEntity(), pyramidTrans);

    const auto func = std::function{ [](const float x, const float y) {
        return glm::sin(x) / (x * x + 1) + glm::sin(y) / (y * y + 1);
    }};
    const auto mesh = Mesh::Builder(func)
            .halfExtentX(4.0f)
            .halfExtentY(4.0f)
            .segments(100)
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::TURQUOISE)
            .build(*engine);

    // Add light to the scene
    const auto globalLight = EntityManager::get()->create();
    LightManager::Builder(LightManager::Type::DIRECTIONAL)
            .direction(1.0f, 0.25f, -0.5f)
            .build(globalLight);

    // Render a small movable aura
    auto auraPos = glm::vec3{6.0f, 6.0f, 8.0f };
    static constexpr auto SPEED = 5.0f;

    // Add a point light
    const auto pointLight = EntityManager::get()->create();
    LightManager::Builder(LightManager::Type::POINT)
            .position(auraPos.x, auraPos.y, auraPos.z)
            .distance(LightManager::LightDistance::AVERAGE)
            .build(pointLight);

    // Manage all transformations
    const auto tm = engine->getTransformManager();

    const auto aura = Aura::Builder().build(*engine);
    const auto auraTrans = translate(glm::mat4(1.0f), auraPos);
    tm->setTransform(aura->getEntity(), auraTrans);

    // Move the aura forward
    context->setOnLongPress(Context::Key::W, [&] {
        const auto velocity = getAuraVelocity(context->getDeltaTimeMillis(), SPEED);
        auraPos += glm::vec3{0.0f, 1.0f, 0.0f } * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tm->setTransform(aura->getEntity(), trans);
    });

    // Move the aura backward
    context->setOnLongPress(Context::Key::S, [&] {
        const auto velocity = getAuraVelocity(context->getDeltaTimeMillis(), SPEED);
        auraPos += glm::vec3{0.0f, -1.0f, 0.0f } * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tm->setTransform(aura->getEntity(), trans);
    });

    // Move the aura left
    context->setOnLongPress(Context::Key::A, [&] {
        const auto velocity = getAuraVelocity(context->getDeltaTimeMillis(), SPEED);
        auraPos += glm::vec3{-1.0f, 0.0f, 0.0f } * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tm->setTransform(aura->getEntity(), trans);
    });

    // Move the aura right
    context->setOnLongPress(Context::Key::D, [&] {
        const auto velocity = getAuraVelocity(context->getDeltaTimeMillis(), SPEED);
        auraPos += glm::vec3{1.0f, 0.0f, 0.0f} * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tm->setTransform(aura->getEntity(), trans);
    });

    // Move the aura up
    context->setOnLongPress(Context::Key::LCTR, [&] {
        const auto velocity = getAuraVelocity(context->getDeltaTimeMillis(), SPEED);
        auraPos += glm::vec3{0.0f, 0.0f, -1.0f } * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tm->setTransform(aura->getEntity(), trans);
    });

    // Move the aura down
    context->setOnLongPress(Context::Key::LSHF, [&] {
        const auto velocity = getAuraVelocity(context->getDeltaTimeMillis(), SPEED);
        auraPos += glm::vec3{0.0f, 0.0f, 1.0f } * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tm->setTransform(aura->getEntity(), trans);
    });

    // Snap the aura back to the initial position
    context->setOnLongPress(Context::Key::I, [&] {
        auraPos = glm::vec3{6.0f, 6.0f, 8.0f };
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tm->setTransform(aura->getEntity(), trans);
    });

    scene->addEntity(cube->getEntity());
    scene->addEntity(frustum->getEntity());
    scene->addEntity(tetra->getEntity());
    scene->addEntity(geoSphere->getEntity());
    scene->addEntity(divSphere->getEntity());
    scene->addEntity(cylinder->getEntity());
    scene->addEntity(cone->getEntity());
    scene->addEntity(pyramid->getEntity());
    scene->addEntity(mesh->getEntity());
    scene->addEntity(aura->getEntity());
    scene->addEntity(globalLight);
    scene->addEntity(pointLight);

    // The render loop
    context->loop([&] { renderer->render(*view); });

    // Free up any resources we may have forgotten to destroy
    engine->destroy();

    return 0;
}

inline float getAuraVelocity(const long deltaTimeMillis, const float speed) {
    return static_cast<float>(deltaTimeMillis) / 1000.0f * speed;
}