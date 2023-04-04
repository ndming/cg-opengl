// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <glm/gtc/matrix_transform.hpp>

#include "Context.h"
#include "Engine.h"
#include "EntityManager.h"
#include "LightManager.h"
#include "Shader.h"

#include "drawable/Cone.h"
#include "drawable/Cube.h"
#include "drawable/Cylinder.h"
#include "drawable/Frustum.h"
#include "drawable/Mesh.h"
#include "drawable/Sphere.h"
#include "drawable/Sun.h"
#include "drawable/Pyramid.h"
#include "drawable/Tetrahedron.h"

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

    // Capture the framebuffer on C press
    // context->setOnPress(Context::Key::C, [&context] {
    //     const auto& [w, h] = context->getFramebufferSize();
    //     const auto image = cv::Mat(h, w, CV_8UC3);
    //     glReadPixels(0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, image.data);
    //     // Flip the image vertically (because OpenGL has the origin in the bottom-left corner)
    //     flip(image, image, 0);
    //
    //     // Save the image as a file
    //     imwrite("captures/scene.png", image);
    // });

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
            .build(*engine);
    const auto cubeTrans = translate(glm::mat4(1.0f), glm::vec3{ 3.0f, 3.0f, 3.0f });
    tcm->setTransform(cube->getEntity(), cubeTrans);

    const auto frustum = Frustum::Builder().build(*engine);
    const auto frustumTrans = translate(glm::mat4(1.0f), glm::vec3{ 3.0f, 3.0f, -3.0f });
    tcm->setTransform(frustum->getEntity(), frustumTrans);

    const auto tetra = Tetrahedron::Builder()
            .shaderModel(Shader::Model::PHONG)
            .build(*engine);
    const auto tetraTrans = translate(glm::mat4(1.0f), glm::vec3{ -3.0f, 3.0f, 3.0f });
    tcm->setTransform(tetra->getEntity(), tetraTrans);

    const auto geoSphere = Sphere::GeographicBuilder()
            .shaderModel(Shader::Model::PHONG)
            .build(*engine);
    const auto geoSphereTrans = translate(glm::mat4(1.0f), glm::vec3{ 3.0f, -3.0f, 3.0f });
    tcm->setTransform(geoSphere->getEntity(), geoSphereTrans);

    const auto divSphere = Sphere::SubdivisionBuilder()
            .shaderModel(Shader::Model::PHONG)
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
    const auto mesh = Mesh::Builder(func)
            .halfExtentX(4.0f)
            .halfExtentY(4.0f)
            .segments(100)
            .shaderModel(Shader::Model::PHONG)
            .build(*engine);

    scene->addEntity(cube->getEntity());
    // scene->addEntity(frustum->getEntity());
    scene->addEntity(tetra->getEntity());
    scene->addEntity(geoSphere->getEntity());
    scene->addEntity(divSphere->getEntity());
    // scene->addEntity(cylinder->getEntity());
    // scene->addEntity(cone->getEntity());
    // scene->addEntity(pyramid->getEntity());
    scene->addEntity(mesh->getEntity());

    // Add light to the scene
    const auto globalLight = EntityManager::get()->create();
    LightManager::Builder(LightManager::Type::DIRECTIONAL)
            .direction(1.0f, 0.25f, -0.5f)
            .build(globalLight);
    // scene->addEntity(globalLight);

    // The sun movement
    static auto sunPos = glm::vec3{ 0.0f, 0.0f, 2.0f };
    static constexpr auto SPEED = 5.0f;

    const auto pointLight = EntityManager::get()->create();
    LightManager::Builder(LightManager::Type::POINT)
            .position(sunPos.x, sunPos.y, sunPos.z)
            .build(pointLight);
    scene->addEntity(pointLight);

    const auto sun = Sun::Builder().build(*engine);
    const auto sunTrans = translate(glm::mat4(1.0f), sunPos);
    tcm->setTransform(sun->getEntity(), sunTrans);
    scene->addEntity(sun->getEntity());

    context->setOnLongPress(Context::Key::W, [&context, &tcm, &sun, &pointLight] {
        const auto velocity = context->getDeltaTime() * SPEED;
        sunPos += glm::vec3{ 0.0f, 1.0f, 0.0f } * velocity;
        const auto trans = translate(glm::mat4(1.0f), sunPos);
        tcm->setTransform(pointLight, trans);
        tcm->setTransform(sun->getEntity(), trans);
    });

    context->setOnLongPress(Context::Key::S, [&context, &tcm, &sun, &pointLight] {
        const auto velocity = context->getDeltaTime() * SPEED;
        sunPos += glm::vec3{ 0.0f, -1.0f, 0.0f } *velocity;
        const auto trans = translate(glm::mat4(1.0f), sunPos);
        tcm->setTransform(pointLight, trans);
        tcm->setTransform(sun->getEntity(), trans);
    });

    context->setOnLongPress(Context::Key::A, [&context, &tcm, &sun, &pointLight] {
        const auto velocity = context->getDeltaTime() * SPEED;
        sunPos += glm::vec3{ -1.0f, 0.0f, 0.0f } *velocity;
        const auto trans = translate(glm::mat4(1.0f), sunPos);
        tcm->setTransform(pointLight, trans);
        tcm->setTransform(sun->getEntity(), trans);
    });

    context->setOnLongPress(Context::Key::D, [&context, &tcm, &sun, &pointLight] {
        const auto velocity = context->getDeltaTime() * SPEED;
        sunPos += glm::vec3{ 1.0f, 0.0f, 0.0f } *velocity;
        const auto trans = translate(glm::mat4(1.0f), sunPos);
        tcm->setTransform(pointLight, trans);
        tcm->setTransform(sun->getEntity(), trans);
    });

    context->setOnLongPress(Context::Key::LCTR, [&context, &tcm, &sun, &pointLight] {
        const auto velocity = context->getDeltaTime() * SPEED;
        sunPos += glm::vec3{ 0.0f, 0.0f, -1.0f } *velocity;
        const auto trans = translate(glm::mat4(1.0f), sunPos);
        tcm->setTransform(pointLight, trans);
        tcm->setTransform(sun->getEntity(), trans);
    });

    context->setOnLongPress(Context::Key::LSHF, [&context, &tcm, &sun, &pointLight] {
        const auto velocity = context->getDeltaTime() * SPEED;
        sunPos += glm::vec3{ 0.0f, 0.0f, 1.0f } *velocity;
        const auto trans = translate(glm::mat4(1.0f), sunPos);
        tcm->setTransform(pointLight, trans);
        tcm->setTransform(sun->getEntity(), trans);
    });

    context->setOnLongPress(Context::Key::SPACE, [&tcm, &sun, &pointLight] {
        sunPos = glm::vec3{ 0.0f, 0.0f, 2.0f };
        const auto trans = translate(glm::mat4(1.0f), sunPos);
        tcm->setTransform(pointLight, trans);
        tcm->setTransform(sun->getEntity(), trans);
    });

    // The render loop
    context->loop([&] { renderer->render(*view); });

    // Free up any resources we may have forgotten to destroy
    engine->destroy();
    return 0;
}