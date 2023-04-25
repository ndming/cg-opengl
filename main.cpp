// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "Context.h"
#include "Engine.h"
#include "EntityManager.h"
#include "Shader.h"

#include "drawable/Orbit.h"
#include "drawable/Sphere.h"

#include "utils/MediaExporter.h"
#include "utils/SolarSystem.h"
#include "utils/TextureLoader.h"

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
    clearOptions.clearColor = { 0.02f, 0.04f, 0.06f, 1.0f };
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
    camera->setRadius(150.0f);
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
        camera->setProjection(45.0f, ratio, 0.1f, 4000.0f);
        view->setViewport({ 0, 0, w, h });
    });

    // Manage all transformations
    const auto tm = engine->getTransformManager();

    // Earth
    const auto earthDiff = loadTexture("earth/earth_diffuse.png", *engine);
    const auto earthSpec = loadTexture("earth/earth_specular.png", *engine);
    const auto earth = Sphere::GeographicBuilder()
            .longitudes(60)
            .latitudes(60)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(earthDiff)
            .textureSpecular(earthSpec)
            .textureShininess(100.0f)
            .build(*engine);
    constexpr auto EARTH_SEMI_MAJOR = 167.0f;
    constexpr auto EARTH_SEMI_MINOR = 160.0f;
    const auto earthX = std::function<float(float)>{ [](const auto angle){
        return EARTH_SEMI_MAJOR * glm::cos(angle);
    }};
    const auto earthY = std::function<float(float)>{ [](const auto angle){
        return EARTH_SEMI_MINOR * glm::sin(angle);
    }};
    constexpr auto EARTH_RADIUS = 3.959f;

    // Moon
    const auto moonDiff = loadTexture("moon/moon_diffuse.jpg", *engine);
    const auto moonSpec = loadTexture("moon/moon_specular.jpg", *engine);
    const auto moon = Sphere::GeographicBuilder()
            .longitudes(60)
            .latitudes(60)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(moonDiff)
            .textureSpecular(moonSpec)
            .textureShininess(100.0f)
            .build(*engine);
    constexpr auto MOON_SEMI_MAJOR = 6.70f;
    constexpr auto MOON_SEMI_MINOR = 6.00f;
    const auto moonX = std::function<float(float)>{ [](const auto angle){
        return MOON_SEMI_MAJOR * glm::cos(angle);
    }};
    const auto moonY = std::function<float(float)>{ [](const auto angle){
        return MOON_SEMI_MINOR * glm::sin(angle);
    }};
    constexpr auto MOON_RADIUS = 0.8f;

    // Sun
    const auto sunTexture = loadTexture("sun/sun_diffuse.jpg", *engine);
    const auto sun = Sphere::GeographicBuilder()
            .longitudes(100)
            .latitudes(100)
            .shaderModel(Shader::Model::UNLIT)
            .textureUnlit(sunTexture)
            .textureShininess(100.0f)
            .build(*engine);
    constexpr auto SUN_RADIUS = 43.2687f;
    auto sunTf = glm::scale(glm::mat4(1.0f), glm::vec3{ SUN_RADIUS, SUN_RADIUS, SUN_RADIUS });
    tm->setTransform(sun->getEntity(), sunTf);

    // Add the sunlight
    const auto sunlight = EntityManager::get()->create();
    LightManager::Builder(LightManager::Type::POINT)
            .position(0.0f, 0.0f, 0.0f)
            .ambient(0.1f, 0.1f, 0.1f)
            .distance(LightManager::LightDistance::MASSIVE)
            .build(sunlight);

    // The Earth orbit
    const auto earthOrbit = Orbit::Builder(earthX, earthY)
            .color(0.09412f, 0.23922f, 0.27843f)
            .segments(1000)
            .build(*engine);

    // The Moon orbit
    const auto moonOrbit = Orbit::Builder(moonX, moonY)
            .color(0.1f, 0.1f, 0.1f)
            .segments(1000)
            .build(*engine);

    // Add renderables to the scene
    scene->addEntity(earth->getEntity());
    scene->addEntity(sun->getEntity());
    scene->addEntity(moon->getEntity());
    scene->addEntity(earthOrbit->getEntity());
    scene->addEntity(moonOrbit->getEntity());
    scene->addEntity(sunlight);

    // The render loop
    context->loop([&] {
        const auto delta = context->getDeltaTimeMillis();

        static auto earthRevolveAngle = 0.0f;
        static auto earthRotateAngle = 0.0f;
        earthRevolveAngle += (0.08f * static_cast<float>(delta) / 1000.0f);
        earthRotateAngle += (5.0f * static_cast<float>(delta) / 1000.0f);
        const auto earthTf = getPlanetTransform(
                earthRevolveAngle, earthRotateAngle, glm::radians(23.5f),
                EARTH_RADIUS,earthX, earthY
        );
        tm->setTransform(earth->getEntity(), earthTf);

        static auto moonRevolveAngle = 0.0f;
        static auto moonRotateAngle = 0.0f;
        moonRevolveAngle += (5.0f * static_cast<float>(delta) / 1000.0f);
        moonRotateAngle += (5.0f * static_cast<float>(delta) / 1000.0f);
        const auto moonOrientation = glm::vec3{ 0.3987f, 0.0f, 0.9171f };
        const auto moonCenter = glm::vec3{ earthX(earthRevolveAngle), earthY(earthRevolveAngle), 0.0f };
        const auto moonTf = getPlanetTransform(
            moonRevolveAngle, moonRotateAngle, glm::radians(6.68f),
            MOON_RADIUS,moonX, moonY, moonOrientation, moonCenter
        );
        tm->setTransform(moon->getEntity(), moonTf);

        const auto moonOrbitTf = getOrbitTransform(moonOrientation, moonCenter);
        tm->setTransform(moonOrbit->getEntity(), moonOrbitTf);

        renderer->render(*view);
    });

    // Destroy all resources
    engine->destroyEntity(earth->getEntity());
    engine->destroyEntity(sun->getEntity());
    engine->destroyEntity(sunlight);
    engine->destroyTexture(earthDiff);
    engine->destroyTexture(earthSpec);
    engine->destroyTexture(sunTexture);
    engine->destroyShader(earth->getShader());
    engine->destroyShader(sun->getShader());
    engine->destroyRenderer(renderer);
    engine->destroyView(view);
    engine->destroyScene(scene);
    engine->destroyCamera(camera->getEntity());

    const auto entityManager = EntityManager::get();
    entityManager->discard(earth->getEntity());
    entityManager->discard(sun->getEntity());
    entityManager->discard(sunlight);

    // Free up any resources we may have forgotten to destroy
    engine->destroy();

    return 0;
}