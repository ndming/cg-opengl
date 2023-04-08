// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#include "Context.h"
#include "Engine.h"
#include "EntityManager.h"
#include "LightManager.h"
#include "Shader.h"

#include "drawable/Material.h"
#include "drawable/Mesh.h"
#include "drawable/Sphere.h"
#include "drawable/Sun.h"

#include "utils/SgdIterator.h"
#include "utils/MediaExporter.h"

glm::mat4 getBallTransform(
    float x, float y, float ballRadius,
    const std::function<float(float, float)>& objective,
    const std::function<float(float, float)>& gradientX,
    const std::function<float(float, float)>& gradientY
);

int main() {
    // The window context
    auto context = Context::create("1952092");

    // Set close on ESC press
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
    clearOptions.clearColor = { 0.01f, 0.02f, 0.03f, 1.0f };
    renderer->setClearOptions(clearOptions);

    // Toggle polygon mode on T press
    context->setOnPress(Context::Key::T, [&renderer] {
        renderer->togglePolygonMode();
    });

    // Capture the framebuffer on C press
    const auto exporter = MediaExporter::Builder().folderPath("capture").build();
    context->setOnPress(Context::Key::C, [&context, &exporter] {
        // Acquire the framebuffer size
        const auto& [w, h] = context->getFramebufferSize();
        // Prepare a placeholder for RGBA image
        const auto data = new unsigned char[w * h * 4];
        // Load the content of the framebuffer
        Renderer::readFramebufferRgba(0, 0, w, h, data);
        // Flip the image vertically since images have the origin at the top left, while OpenGL expects the origin
        // at the bottom left.
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
    // Pan the camera around the looking sphere
    context->setMouseDragPerpetualCallback([&camera](const auto offsetX, const auto offsetY) {
        camera->relativeDrag(offsetX, offsetY);
    });

    // Create a scene
    const auto scene = engine->createScene();

    // Create and set up a view
    const auto view = engine->createView();
    view->setCamera(camera);
    view->setScene(scene);

    // Update the projection and viewport on window resize
    context->setFramebufferCallback([&](const auto w, const auto h) {
        const auto ratio = static_cast<float>(w) / static_cast<float>(h);
        camera->setProjection(45.0f, ratio, 0.1f, 100.0f);
        view->setViewport({ 0, 0, w, h });
    });

    // Manage all transformations
    const auto tcm = engine->getTransformManager();

    // The rolling ball
    const auto ball = Sphere::SubdivisionBuilder()
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::PEARL)
            .build(*engine);
    static constexpr auto ballRadius = 0.4f;
    // Scale the ball down to this radius
    const auto ballTrans = glm::scale(glm::mat4(1.0f), glm::vec3{ ballRadius, ballRadius, ballRadius });
    tcm->setTransform(ball->getEntity(), ballTrans);

    // The objective function
    const auto objective = std::function{ [](const float x, const float y) {
        return (x*x + y*y) / 40.0f + 2.0f * std::exp(-(std::cos(x / 2.0f) + y * y / 4.0f)) - std::cos(x / 1.5f) - std::sin(y / 1.5f);
    }};
    // Gradient with respect to x
    const auto gradientX = std::function{ [](const float x, const float y) {
        return (x / 20.0f) + std::exp(-(std::cos(x / 2.0f) + y * y / 4.0f)) * std::sin(x / 2.0f) + (std::sin(x / 1.5f) / 1.5f);
    }};
    // Gradient with respect to y
    const auto gradientY = std::function{ [](const float x, const float y) {
        return (y / 20.0f) - std::exp(-(std::cos(x / 2.0f) + y * y / 4.0f)) * y - (std::cos(y / 1.5f) / 1.5f);
    }};

    // The mesh
    const auto mesh = Mesh::Builder(objective)
            .halfExtent(10.0f)
            .segments(100)
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::JADE)
            .build(*engine);

    // The SGD iterator
    auto sgd = SgdIterator::Builder()
            .gradientX(gradientX)
            .gradientY(gradientY)
            .convergenceRate(0.1f)
            .build();
    // The initial position of the ball
    sgd->resetState(0.0f, 0.0f);
    // Transform the ball
    const auto trans = getBallTransform(0.0f, 0.0f, ballRadius, objective, gradientX, gradientY);
    tcm->setTransform(ball->getEntity(), trans);

    // Random the SGD state on R press
    context->setOnPress(Context::Key::R, [&sgd, &objective, &tcm, &ball, &gradientX, &gradientY]{
        // Random the position of the ball on the mesh
        sgd->randomState(8.0f, 8.0f);
        // Get the random position
        const auto [x, y] = sgd->getState();
        // Transform the ball
        const auto trans = getBallTransform(x, y, ballRadius, objective, gradientX, gradientY);
        tcm->setTransform(ball->getEntity(), trans);
    });

    // Manually descent the ball on I press
    context->setOnLongPress(Context::Key::I, [&sgd, &objective, &tcm, &ball, &gradientX, &gradientY]{
        sgd->iterate();
        // Get the new position
        const auto [x, y] = sgd->getState();
        // Transform the ball
        const auto trans = getBallTransform(x, y, ballRadius, objective, gradientX, gradientY);
        tcm->setTransform(ball->getEntity(), trans);
    });

    // Add a global light
    const auto globalLight = EntityManager::get()->create();
    LightManager::Builder(LightManager::Type::DIRECTIONAL)
            .direction(1.0f, 0.25f, -0.5f)
            .ambient(0.08f, 0.08f, 0.08f)
            .build(globalLight);

    // Render a small movable aura
    static auto auraPos = glm::vec3{4.0f, 4.0f, 8.0f };
    static constexpr auto SPEED = 5.0f;

    // Add a point light
    const auto pointLight = EntityManager::get()->create();
    LightManager::Builder(LightManager::Type::POINT)
            .position(auraPos.x, auraPos.y, auraPos.z)
            .build(pointLight);

    const auto aura = Sun::Builder().build(*engine);
    const auto auraTrans = translate(glm::mat4(1.0f), auraPos);
    tcm->setTransform(aura->getEntity(), auraTrans);

    // Move the aura forward
    context->setOnLongPress(Context::Key::W, [&] {
        const auto velocity = context->getDeltaTime() * SPEED;
        auraPos += glm::vec3{0.0f, 1.0f, 0.0f } * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tcm->setTransform(aura->getEntity(), trans);
    });

    // Move the aura backward
    context->setOnLongPress(Context::Key::S, [&] {
        const auto velocity = context->getDeltaTime() * SPEED;
        auraPos += glm::vec3{0.0f, -1.0f, 0.0f } * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tcm->setTransform(aura->getEntity(), trans);
    });

    // Move the aura left
    context->setOnLongPress(Context::Key::A, [&] {
        const auto velocity = context->getDeltaTime() * SPEED;
        auraPos += glm::vec3{-1.0f, 0.0f, 0.0f } * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tcm->setTransform(aura->getEntity(), trans);
    });

    // Move the aura right
    context->setOnLongPress(Context::Key::D, [&] {
        const auto velocity = context->getDeltaTime() * SPEED;
        auraPos += glm::vec3{1.0f, 0.0f, 0.0f} * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tcm->setTransform(aura->getEntity(), trans);
    });

    // Move the aura up
    context->setOnLongPress(Context::Key::LCTR, [&] {
        const auto velocity = context->getDeltaTime() * SPEED;
        auraPos += glm::vec3{0.0f, 0.0f, -1.0f } * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tcm->setTransform(aura->getEntity(), trans);
    });

    // Move the aura down
    context->setOnLongPress(Context::Key::LSHF, [&] {
        const auto velocity = context->getDeltaTime() * SPEED;
        auraPos += glm::vec3{0.0f, 0.0f, 1.0f } * velocity;
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tcm->setTransform(aura->getEntity(), trans);
    });

    // Snap the aura back to the initial position
    context->setOnLongPress(Context::Key::SPACE, [&] {
        auraPos = glm::vec3{4.0f, 4.0f, 8.0f };
        engine->getLightManager()->setPosition(pointLight, auraPos.x, auraPos.y, auraPos.z);
        const auto trans = translate(glm::mat4(1.0f), auraPos);
        tcm->setTransform(aura->getEntity(), trans);
    });

    // Add renderables to the scene
    scene->addEntity(ball->getEntity());
    scene->addEntity(mesh->getEntity());
    scene->addEntity(aura->getEntity());
    scene->addEntity(pointLight);
    scene->addEntity(globalLight);

    // The render loop
    context->loop([&] { renderer->render(*view); });

    // Destroy all resources
    engine->destroyEntity(ball->getEntity());
    engine->destroyEntity(mesh->getEntity());
    engine->destroyEntity(aura->getEntity());
    engine->destroyEntity(globalLight);
    engine->destroyEntity(pointLight);
    engine->destroyShader(ball->getShader());
    engine->destroyShader(mesh->getShader());
    engine->destroyShader(aura->getShader());
    engine->destroyRenderer(renderer);
    engine->destroyView(view);
    engine->destroyScene(scene);
    engine->destroyCamera(camera->getEntity());

    const auto entityManager = EntityManager::get();
    entityManager->discard(ball->getEntity());
    entityManager->discard(mesh->getEntity());
    entityManager->discard(aura->getEntity());
    entityManager->discard(globalLight);
    entityManager->discard(pointLight);

    // Free up any resources we may have forgotten to destroy
    engine->destroy();

    return 0;
}

glm::mat4 getBallTransform(
    const float x, const float y, const float ballRadius,
    const std::function<float(float, float)>& objective,
    const std::function<float(float, float)>& gradientX,
    const std::function<float(float, float)>& gradientY
) {
    const auto z = objective(x, y);
    // Get the normal vector at this position
    const auto dirX = gradientX(x, y);
    const auto dirY = gradientY(x, y);
    const auto norm = -glm::normalize(glm::vec3{ dirX, dirY, -1.0f });
    // Transform the ball, while maintaining the scaling
    auto trans = glm::translate(glm::mat4(1.0f), norm * ballRadius);
    trans = glm::translate(trans, glm::vec3{ x, y, z });
    trans = glm::scale(trans, glm::vec3{ ballRadius, ballRadius, ballRadius });
    return trans;
}