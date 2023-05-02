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

// Sun
constexpr auto SUN_RADIUS = 23.2523f; // using AU, multiplied by (just) 5000

// Mercury
constexpr auto MERCURY_SEMI_MAJOR = 38.71f; // using AU, multiplied by 100
constexpr auto MERCURY_SEMI_MINOR = 38.28f; // using AU, multiplied by 100
constexpr auto MERCURY_RADIUS = 0.8154f;    // using AU, multiplied by 50000
constexpr auto MERCURY_TILTING = 0.01f;     // degree
constexpr auto MERCURY_REVOLVING_SPEED = 0.1277f; // km/s, scaled by 375
constexpr auto MERCURY_ROTATING_SPEED = 0.0289f;  // km/h, scaled by 375

// Venus
constexpr auto VENUS_SEMI_MAJOR = 72.33f; // using AU, multiplied by 100
constexpr auto VENUS_SEMI_MINOR = 71.84f; // using AU, multiplied by 100
constexpr auto VENUS_RADIUS = 2.0227f;    // using AU, multiplied by 50000
constexpr auto VENUS_TILTING = 177.36f;     // degree
constexpr auto VENUS_REVOLVING_SPEED = 0.0934f; // km/s, scaled by 375
constexpr auto VENUS_ROTATING_SPEED = 0.0174f;  // km/h, scaled by 375

// Earth
constexpr auto EARTH_SEMI_MAJOR = 100.0f; // using AU, multiplied by 100
constexpr auto EARTH_SEMI_MINOR = 99.0f;  // using AU, multiplied by 100
constexpr auto EARTH_RADIUS = 2.1294f;    // using AU, multiplied by 50000
constexpr auto EARTH_TILTING = 23.5f;     // degree
constexpr auto EARTH_REVOLVING_SPEED = 0.0793f; // km/s, scaled by 375
constexpr auto EARTH_ROTATING_SPEED = 4.4651f;  // km/h, scaled by 375

// Mars
constexpr auto MARS_SEMI_MAJOR = 152.37f; // using AU, multiplied by 100
constexpr auto MARS_SEMI_MINOR = 151.70f; // using AU, multiplied by 100
constexpr auto MARS_RADIUS = 1.1329f;     // using AU, multiplied by 50000
constexpr auto MARS_TILTING = 25.19f;      // degree
constexpr auto MARS_REVOLVING_SPEED = 0.0642f; // km/s, scaled by 375
constexpr auto MARS_ROTATING_SPEED = 2.3153f;  // km/h, scaled by 375

// Jupiter
constexpr auto JUPITER_SEMI_MAJOR = 520.28f; // using AU, multiplied by 100
constexpr auto JUPITER_SEMI_MINOR = 519.58f; // using AU, multiplied by 100
constexpr auto JUPITER_RADIUS = 23.3663f;    // using AU, multiplied by 50000
constexpr auto JUPITER_TILTING = 3.13f;      // degree
constexpr auto JUPITER_REVOLVING_SPEED = 0.0349f;  // km/s, scaled by 375
constexpr auto JUPITER_ROTATING_SPEED = 125.3387f; // km/h, scaled by 375

// Saturn
constexpr auto SATURN_SEMI_MAJOR = 953.88f; // using AU, multiplied by 100
constexpr auto SATURN_SEMI_MINOR = 951.11f; // using AU, multiplied by 100
constexpr auto SATURN_RADIUS = 19.4628f;    // using AU, multiplied by 50000
constexpr auto SATURN_TILTING = 26.73f;     // degree
constexpr auto SATURN_REVOLVING_SPEED = 0.0258f; // km/s, scaled by 375
constexpr auto SATURN_ROTATING_SPEED = 92.856f;  // km/h, scaled by 375

// Uranus
constexpr auto URANUS_SEMI_MAJOR = 1919.14f; // using AU, multiplied by 100
constexpr auto URANUS_SEMI_MINOR = 1916.50f; // using AU, multiplied by 100
constexpr auto URANUS_RADIUS = 8.4767f;      // using AU, multiplied by 50000
constexpr auto URANUS_TILTING = 97.77f;      // degree
constexpr auto URANUS_REVOLVING_SPEED = 0.0182f; // km/s, scaled by 375
constexpr auto URANUS_ROTATING_SPEED = 25.008f;  // km/h, scaled by 375

// Neptune
constexpr auto NEPTUNE_SEMI_MAJOR = 3006.11f; // using AU, multiplied by 100
constexpr auto NEPTUNE_SEMI_MINOR = 3003.34f; // using AU, multiplied by 100
constexpr auto NEPTUNE_RADIUS = 8.2294f;      // using AU, multiplied by 50000
constexpr auto NEPTUNE_TILTING = 28.32f;      // degree
constexpr auto NEPTUNE_REVOLVING_SPEED = 0.0145f; // km/s, scaled by 375
constexpr auto NEPTUNE_ROTATING_SPEED = 25.917f;  // km/h, scaled by 375

// Moon
constexpr auto MOON_SEMI_MAJOR = 0.256f + EARTH_RADIUS * 2; // padded by the Earth's diameter since scales are different
constexpr auto MOON_SEMI_MINOR = 0.256f + EARTH_RADIUS * 2; // padded by the Earth's diameter since scales are different
constexpr auto MOON_RADIUS = 0.5806f; // using AU, multiplied by 50000
constexpr auto MOON_TILTING = 6.68f;  // degree
constexpr auto MOON_REVOLVING_SPEED = EARTH_ROTATING_SPEED;
constexpr auto MOON_ROTATING_SPEED = EARTH_ROTATING_SPEED / 27.3;

inline float getDeltaAngle(float speed, long frameDelta);

int main() {
    // The window context
    auto context = Context::create("Solar System");

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
    camera->setRadius(250.0f);
    camera->setZoomSensitive(10.0f);
    camera->setDragSensitive(0.2f);
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
        camera->setProjection(45.0f, ratio, 0.1f, 8000.0f);
        view->setViewport({ 0, 0, w, h });
    });

    // Manage all transformations
    const auto tm = engine->getTransformManager();

    // Sun
    const auto sunTexture = loadTexture("sun/sun_diffuse_2k.jpg", *engine);
    const auto sun = Sphere::GeographicBuilder()
            .longitudes(60)
            .latitudes(60)
            .shaderModel(Shader::Model::UNLIT)
            .textureUnlit(sunTexture)
            .build(*engine);
    auto sunTf = glm::scale(glm::mat4(1.0f), glm::vec3{ SUN_RADIUS, SUN_RADIUS, SUN_RADIUS });
    tm->setTransform(sun->getEntity(), sunTf);

    // Add the sunlight
    const auto sunlight = EntityManager::get()->create();
    LightManager::Builder(LightManager::Type::POINT)
            .position(0.0f, 0.0f, 0.0f)
            .ambient(0.08f, 0.08f, 0.08f)
            .distance(LightManager::LightDistance::SUPER_MASSIVE)
            .build(sunlight);

    // Mercury
    const auto mercuryDiff = loadTexture("mercury/mercury_diffuse_2k.jpg", *engine);
    const auto mercury = Sphere::GeographicBuilder()
            .longitudes(10)
            .latitudes(10)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(mercuryDiff)
            .build(*engine);
    const auto mercuryX = std::function<float(float)>{ [](const auto angle){ return MERCURY_SEMI_MAJOR * glm::cos(angle); }};
    const auto mercuryY = std::function<float(float)>{ [](const auto angle){ return MERCURY_SEMI_MINOR * glm::sin(angle); }};
    const auto mercuryOrientation = glm::vec3{ -0.1219f, 0.0f, 0.9925f };

    // Mercury orbit
    const auto mercuryOrbit = Orbit::Builder(mercuryX, mercuryY)
            .color(0.65490f, 0.64314f, 0.62353f)
            .segments(200)
            .build(*engine);
    tm->setTransform(mercuryOrbit->getEntity(), getOrbitTransform(mercuryOrientation));

    // Mercury
    const auto venusDiff = loadTexture("venus/venus_diffuse_2k.jpg", *engine);
    const auto venus = Sphere::GeographicBuilder()
            .longitudes(20)
            .latitudes(20)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(venusDiff)
            .build(*engine);
    const auto venusX = std::function<float(float)>{ [](const auto angle){ return VENUS_SEMI_MAJOR * glm::cos(angle); }};
    const auto venusY = std::function<float(float)>{ [](const auto angle){ return VENUS_SEMI_MINOR * glm::sin(angle); }};
    const auto venusOrientation = glm::vec3{ -0.0592f, 0.0f, 0.9982f };

    // Mercury orbit
    const auto venusOrbit = Orbit::Builder(venusX, venusY)
            .color(0.64314f, 0.52549f, 0.34902f)
            .segments(300)
            .build(*engine);
    tm->setTransform(venusOrbit->getEntity(), getOrbitTransform(venusOrientation));

    // Earth
    const auto earthDiff = loadTexture("earth/earth_diffuse.png", *engine);
    const auto earthSpec = loadTexture("earth/earth_specular.png", *engine);
    const auto earth = Sphere::GeographicBuilder()
            .longitudes(40)
            .latitudes(40)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(earthDiff)
            .textureSpecular(earthSpec)
            .textureShininess(100.0f)
            .build(*engine);
    const auto earthX = std::function<float(float)>{ [](const auto angle){ return EARTH_SEMI_MAJOR * glm::cos(angle); }};
    const auto earthY = std::function<float(float)>{ [](const auto angle){ return EARTH_SEMI_MINOR * glm::sin(angle); }};

    // The Earth orbit
    const auto earthOrbit = Orbit::Builder(earthX, earthY)
            .color(0.21961f, 0.46275f, 0.47059f)
            .segments(400)
            .build(*engine);

    // Mars
    const auto marsDiff = loadTexture("mars/mars_diffuse_2k.jpg", *engine);
    const auto mars = Sphere::GeographicBuilder()
            .longitudes(40)
            .latitudes(40)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(marsDiff)
            .build(*engine);
    const auto marsX = std::function<float(float)>{ [](const auto angle){ return MARS_SEMI_MAJOR * glm::cos(angle); }};
    const auto marsY = std::function<float(float)>{ [](const auto angle){ return MARS_SEMI_MINOR * glm::sin(angle); }};
    const auto marsOrientation = glm::vec3{ -0.0323f, 0.0f, 0.9995f };

    // Mars orbit
    const auto marsOrbit = Orbit::Builder(marsX, marsY)
            .color(0.45882f, 0.18824f, 0.07451f)
            .segments(500)
            .build(*engine);
    tm->setTransform(marsOrbit->getEntity(), getOrbitTransform(marsOrientation));

    // Jupiter
    const auto jupiterDiff = loadTexture("jupiter/jupiter_diffuse_2k.jpg", *engine);
    const auto jupiter = Sphere::GeographicBuilder()
            .longitudes(80)
            .latitudes(80)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(jupiterDiff)
            .build(*engine);
    const auto jupiterX = std::function<float(float)>{ [](const auto angle){ return JUPITER_SEMI_MAJOR * glm::cos(angle); }};
    const auto jupiterY = std::function<float(float)>{ [](const auto angle){ return JUPITER_SEMI_MINOR * glm::sin(angle); }};
    const auto jupiterOrientation = glm::vec3{ -0.0228f, 0.0f, 0.9997f };

    // Jupiter orbit
    const auto jupiterOrbit = Orbit::Builder(jupiterX, jupiterY)
            .color(0.72157f, 0.64706f, 0.55294f)
            .segments(600)
            .build(*engine);
    tm->setTransform(jupiterOrbit->getEntity(), getOrbitTransform(jupiterOrientation));

    // Saturn
    const auto saturnDiff = loadTexture("saturn/saturn_diffuse_2k.jpg", *engine);
    const auto saturn = Sphere::GeographicBuilder()
            .longitudes(80)
            .latitudes(80)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(saturnDiff)
            .build(*engine);
    const auto saturnX = std::function<float(float)>{ [](const auto angle){ return SATURN_SEMI_MAJOR * glm::cos(angle); }};
    const auto saturnY = std::function<float(float)>{ [](const auto angle){ return SATURN_SEMI_MINOR * glm::sin(angle); }};
    const auto saturnOrientation = glm::vec3{ -0.0433f, 0.0f, 0.9991f };

    // Saturn orbit
    const auto saturnOrbit = Orbit::Builder(saturnX, saturnY)
            .color(0.73725f, 0.76078f, 0.62353f)
            .segments(700)
            .build(*engine);
    tm->setTransform(saturnOrbit->getEntity(), getOrbitTransform(saturnOrientation));

    // Uranus
    const auto uranusDiff = loadTexture("uranus/uranus_diffuse_2k.jpg", *engine);
    const auto uranus = Sphere::GeographicBuilder()
            .longitudes(60)
            .latitudes(60)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(uranusDiff)
            .build(*engine);
    const auto uranusX = std::function<float(float)>{ [](const auto angle){ return URANUS_SEMI_MAJOR * glm::cos(angle); }};
    const auto uranusY = std::function<float(float)>{ [](const auto angle){ return URANUS_SEMI_MINOR * glm::sin(angle); }};
    const auto uranusOrientation = glm::vec3{ -0.0138f, 0.0f, 0.9999f };

    // Uranus orbit
    const auto uranusOrbit = Orbit::Builder(uranusX, uranusY)
            .color(0.60784f, 0.78039f, 0.76471f)
            .segments(800)
            .build(*engine);
    tm->setTransform(uranusOrbit->getEntity(), getOrbitTransform(uranusOrientation));

    // Neptune
    const auto neptuneDiff = loadTexture("neptune/neptune_diffuse_2k.jpg", *engine);
    const auto neptune = Sphere::GeographicBuilder()
            .longitudes(60)
            .latitudes(60)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(neptuneDiff)
            .build(*engine);
    const auto neptuneX = std::function<float(float)>{ [](const auto angle){ return NEPTUNE_SEMI_MAJOR * glm::cos(angle); }};
    const auto neptuneY = std::function<float(float)>{ [](const auto angle){ return NEPTUNE_SEMI_MINOR * glm::sin(angle); }};
    const auto neptuneOrientation = glm::vec3{ -0.0309f, 0.0f, 0.9995f };

    // Neptune orbit
    const auto neptuneOrbit = Orbit::Builder(neptuneX, neptuneY)
            .color(0.02745f, 0.21569f, 0.46275f)
            .segments(900)
            .build(*engine);
    tm->setTransform(neptuneOrbit->getEntity(), getOrbitTransform(neptuneOrientation));

    // Moon
    const auto moonDiff = loadTexture("moon/moon_diffuse.jpg", *engine);
    const auto moon = Sphere::GeographicBuilder()
            .longitudes(20)
            .latitudes(20)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(moonDiff)
            .build(*engine);
    const auto moonX = std::function<float(float)>{ [](const auto angle){ return MOON_SEMI_MAJOR * glm::cos(angle); }};
    const auto moonY = std::function<float(float)>{ [](const auto angle){ return MOON_SEMI_MINOR * glm::sin(angle); }};

    // The Moon orbit
    const auto moonOrbit = Orbit::Builder(moonX, moonY)
            .color(0.1f, 0.1f, 0.1f)
            .segments(100)
            .build(*engine);

    // Add renderables to the scene
    scene->addEntity(sun->getEntity());
    scene->addEntity(sunlight);
    scene->addEntity(mercury->getEntity());
    scene->addEntity(mercuryOrbit->getEntity());
    scene->addEntity(venus->getEntity());
    scene->addEntity(venusOrbit->getEntity());
    scene->addEntity(earth->getEntity());
    scene->addEntity(earthOrbit->getEntity());
    scene->addEntity(mars->getEntity());
    scene->addEntity(marsOrbit->getEntity());
    scene->addEntity(jupiter->getEntity());
    scene->addEntity(jupiterOrbit->getEntity());
    scene->addEntity(saturn->getEntity());
    scene->addEntity(saturnOrbit->getEntity());
    scene->addEntity(uranus->getEntity());
    scene->addEntity(uranusOrbit->getEntity());
    scene->addEntity(neptune->getEntity());
    scene->addEntity(neptuneOrbit->getEntity());
    scene->addEntity(moon->getEntity());
    scene->addEntity(moonOrbit->getEntity());

    // A temporary workaround to make the orbits less jagged
    glLineWidth(1.5f);

    // The render loop
    context->loop([&] {
        const auto delta = context->getDeltaTimeMillis();

        static auto mercuryRevolveAngle = 0.0f;
        static auto mercuryRotateAngle = 0.0f;
        mercuryRevolveAngle += getDeltaAngle(MERCURY_REVOLVING_SPEED, delta);
        mercuryRotateAngle += getDeltaAngle(MERCURY_ROTATING_SPEED, delta);
        const auto mercuryTf = getPlanetTransform(
            mercuryRevolveAngle,mercuryRotateAngle, glm::radians(MERCURY_TILTING),
            MERCURY_RADIUS,mercuryX, mercuryY, mercuryOrientation
        );
        tm->setTransform(mercury->getEntity(), mercuryTf);

        static auto venusRevolveAngle = 0.0f;
        static auto venusRotateAngle = 0.0f;
        venusRevolveAngle += getDeltaAngle(VENUS_REVOLVING_SPEED, delta);
        venusRotateAngle += getDeltaAngle(VENUS_ROTATING_SPEED, delta);
        const auto venusTf = getPlanetTransform(
            venusRevolveAngle,venusRotateAngle, glm::radians(VENUS_TILTING),
            VENUS_RADIUS,venusX, venusY, venusOrientation
        );
        tm->setTransform(venus->getEntity(), venusTf);

        static auto earthRevolveAngle = 0.0f;
        static auto earthRotateAngle = 0.0f;
        earthRevolveAngle += getDeltaAngle(EARTH_REVOLVING_SPEED, delta);
        earthRotateAngle += getDeltaAngle(EARTH_ROTATING_SPEED, delta);
        const auto earthTf = getPlanetTransform(
            earthRevolveAngle, earthRotateAngle, glm::radians(EARTH_TILTING),
            EARTH_RADIUS,earthX, earthY
        );
        tm->setTransform(earth->getEntity(), earthTf);

        static auto marsRevolveAngle = 0.0f;
        static auto marsRotateAngle = 0.0f;
        marsRevolveAngle += getDeltaAngle(MARS_REVOLVING_SPEED, delta);
        marsRotateAngle += getDeltaAngle(MARS_ROTATING_SPEED, delta);
        const auto marsTf = getPlanetTransform(
            marsRevolveAngle,marsRotateAngle, glm::radians(MARS_TILTING),
            MARS_RADIUS,marsX, marsY, marsOrientation
        );
        tm->setTransform(mars->getEntity(), marsTf);

        static auto jupiterRevolveAngle = 0.0f;
        static auto jupiterRotateAngle = 0.0f;
        jupiterRevolveAngle += getDeltaAngle(JUPITER_REVOLVING_SPEED, delta);
        jupiterRotateAngle += getDeltaAngle(JUPITER_ROTATING_SPEED, delta);
        const auto jupiterTf = getPlanetTransform(
            jupiterRevolveAngle,jupiterRotateAngle, glm::radians(JUPITER_TILTING),
            JUPITER_RADIUS,jupiterX, jupiterY, jupiterOrientation
        );
        tm->setTransform(jupiter->getEntity(), jupiterTf);

        static auto saturnRevolveAngle = 0.0f;
        static auto saturnRotateAngle = 0.0f;
        saturnRevolveAngle += getDeltaAngle(SATURN_REVOLVING_SPEED, delta);
        saturnRotateAngle += getDeltaAngle(SATURN_ROTATING_SPEED, delta);
        const auto saturnTf = getPlanetTransform(
            saturnRevolveAngle,saturnRotateAngle, glm::radians(SATURN_TILTING),
            SATURN_RADIUS,saturnX, saturnY, saturnOrientation
        );
        tm->setTransform(saturn->getEntity(), saturnTf);

        static auto uranusRevolveAngle = 0.0f;
        static auto uranusRotateAngle = 0.0f;
        uranusRevolveAngle += getDeltaAngle(URANUS_REVOLVING_SPEED, delta);
        uranusRotateAngle += getDeltaAngle(URANUS_ROTATING_SPEED, delta);
        const auto uranusTf = getPlanetTransform(
            uranusRevolveAngle,uranusRotateAngle, glm::radians(URANUS_TILTING),
            URANUS_RADIUS,uranusX, uranusY, uranusOrientation
        );
        tm->setTransform(uranus->getEntity(), uranusTf);

        static auto neptuneRevolveAngle = 0.0f;
        static auto neptuneRotateAngle = 0.0f;
        neptuneRevolveAngle += getDeltaAngle(NEPTUNE_REVOLVING_SPEED, delta);
        neptuneRotateAngle += getDeltaAngle(NEPTUNE_ROTATING_SPEED, delta);
        const auto neptuneTf = getPlanetTransform(
            neptuneRevolveAngle,neptuneRotateAngle, glm::radians(NEPTUNE_TILTING),
            NEPTUNE_RADIUS,neptuneX, neptuneY, neptuneOrientation
        );
        tm->setTransform(neptune->getEntity(), neptuneTf);

        static auto moonRevolveAngle = 0.0f;
        static auto moonRotateAngle = 0.0f;
        moonRevolveAngle += getDeltaAngle(MOON_REVOLVING_SPEED, delta);
        moonRotateAngle += getDeltaAngle(MOON_ROTATING_SPEED, delta);
        // The Moon inclines about 5 degrees above the ecliptic plane.
        const auto moonOrientation = glm::vec3{ 0.0872f, 0.0f, 0.9962f };
        const auto moonCenter = glm::vec3{ earthX(earthRevolveAngle), earthY(earthRevolveAngle), 0.0f };
        const auto moonTf = getPlanetTransform(
            moonRevolveAngle, moonRotateAngle, glm::radians(MOON_TILTING),
            MOON_RADIUS,moonX, moonY, moonOrientation, moonCenter
        );
        tm->setTransform(moon->getEntity(), moonTf);

        const auto moonOrbitTf = getOrbitTransform(moonOrientation, moonCenter);
        tm->setTransform(moonOrbit->getEntity(), moonOrbitTf);

        renderer->render(*view);
    });

    // Destroy all resources
    engine->destroyRenderer(renderer);
    engine->destroyView(view);
    engine->destroyScene(scene);
    engine->destroyCamera(camera->getEntity());

    // Free up any resources we may have forgotten to destroy
    engine->destroy();

    return 0;
}

inline float getDeltaAngle(const float speed, const long frameDelta) {
    return speed * (static_cast<float>(frameDelta) / 1000.0f);
}