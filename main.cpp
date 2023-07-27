// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Context.h"
#include "Engine.h"
#include "EntityManager.h"
#include "LightManager.h"
#include "Shader.h"
#include "Skybox.h"

#include "drawable/Contour.h"
#include "drawable/Material.h"
#include "drawable/Mesh.h"
#include "drawable/Sphere.h"
#include "drawable/Aura.h"

#include "utils/ContourTracer.h"
#include "utils/DescentIterator.h"
#include "utils/DescentTracer.h"
#include "utils/MediaExporter.h"
#include "utils/TextureLoader.h"

glm::mat4 getBallTransform(
    float x, float y, float ballRadius, float distance,
    const std::function<float(float, float)>& objective,
    const std::function<float(float, float)>& gradientX,
    const std::function<float(float, float)>& gradientY
);

glm::mat4 getContourBallTransform(float x, float y, float radius);

static float ballAngle{ 0.0f };
constexpr auto MESH_HALF_EXTENT = 10.0f;
constexpr auto HEAT_VIEW_MIN_HE = MESH_HALF_EXTENT + 4.0f;

bool isMouseClickInContour(
    float x, float y, int frameWidth, int frameHeight,
    float mapHalfExtent, float viewMinHalfExtent, float phi, float initialPhi,
    float* outX, float* outY
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
    camera->setRadius(MESH_HALF_EXTENT * 3.0f);
    camera->setLatitudeAngle(45.0f);

    // Create another camera for the contour view
    const auto contourCam = engine->createCamera(EntityManager::get()->create());
    contourCam->setRadius(1.0f);
    contourCam->setLatitudeAngle(0.0f);
    contourCam->setLongitudeAngle(-90.0f);

    Context::setOnMouseScroll([&](const auto offsetY) {
        const auto& [width, height] = context->getFramebufferSize();
        const auto& [xPos, yPos] = context->getMousePos();
        if (xPos < static_cast<float>(width) / 2.0f) {
            camera->relativeZoom(offsetY);
        } else {
            contourCam->relativeZoom(offsetY);
        }
    });

    Context::setOnMouseDragPerpetual([&](const auto offsetX, const auto offsetY) {
        const auto& [width, height] = context->getFramebufferSize();
        const auto& [xPos, yPos] = context->getMousePos();
        if (xPos < static_cast<float>(width) / 2.0f) {
            camera->relativeDrag(offsetX, offsetY);
            contourCam->relativeDrag(offsetX, 0.0f);
        }
    });

    // Create a scene
    const auto scene = engine->createScene();

    // Create and set up a view
    const auto view = engine->createView();
    // Create a skybox
    const auto skybox = Skybox::Builder()
            .color(0.02f, 0.04f, 0.06f, 1.0f)
            .build(*engine);
    view->setSkybox(skybox);
    view->setCamera(camera);
    view->setScene(scene);

    // Create another scene
    const auto contourScene = engine->createScene();

    // Create and set up another view
    const auto contourView = engine->createView();
    // Create another skybox
    const auto contourSkybox = Skybox::Builder()
            .color(0.09804f, 0.14118f, 0.15686f, 1.0f)
            .build(*engine);
    contourView->setSkybox(contourSkybox);
    contourView->setCamera(contourCam);
    contourView->setScene(contourScene);

    // Update the projection and viewport on window resize
    context->setFramebufferCallback([&](const auto w, const auto h) {
        const auto ratio = (static_cast<float>(w) / 2.0f) / static_cast<float>(h);
        camera->setProjection(45.0f, ratio, 0.1f, 100.0f);
        view->setViewport({ 0, 0, w / 2, h });

        if ((w / 2) > h) {
            const auto cRatio = (static_cast<float>(w) / 2.0f) / static_cast<float>(h);
            contourCam->setProjection(
                    -HEAT_VIEW_MIN_HE * cRatio, HEAT_VIEW_MIN_HE * cRatio,
                    -HEAT_VIEW_MIN_HE, HEAT_VIEW_MIN_HE, 0.0f, 100.0f
            );
        } else {
            const auto cRatio = static_cast<float>(h) / (static_cast<float>(w) / 2.0f);
            contourCam->setProjection(
                    -HEAT_VIEW_MIN_HE, HEAT_VIEW_MIN_HE,
                    -HEAT_VIEW_MIN_HE * cRatio, HEAT_VIEW_MIN_HE * cRatio, 0.0f, 100.0f
            );
        }
        contourView->setViewport({ w / 2, 0, w / 2, h });
    });

    // Manage all transformations
    const auto tm = engine->getTransformManager();

    // The rolling ball
    const auto ballDiff = loadTexture("softball/softball_diffuse.jpg", *engine);
    const auto ballSpec = loadTexture("softball/softball_bump.jpg", *engine);
    const auto ball = Sphere::GeographicBuilder()
            .longitudes(60)
            .latitudes(60)
            .shaderModel(Shader::Model::PHONG)
            .textureDiffuse(ballDiff)
            .textureSpecular(ballSpec)
            .textureShininess(80.0f)
            .build(*engine);
    static constexpr auto ballRadius = 0.4f;
    // Scale the ball down to this radius
    const auto ballTrans = glm::scale(glm::mat4(1.0f), glm::vec3{ ballRadius, ballRadius, ballRadius });
    tm->setTransform(ball->getEntity(), ballTrans);

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
            .halfExtent(MESH_HALF_EXTENT)
            .segments(100)
            .shaderModel(Shader::Model::PHONG)
            .phongMaterial(phong::TURQUOISE)
            .build(*engine);

    // The SGD iterator
    auto sgd = DescentIterator::Builder()
            .gradientX(gradientX)
            .gradientY(gradientY)
            .convergenceRate(0.08f)
            .build();
    // The initial position of the ball
    sgd->resetState(0.0f, 0.0f);
    // Transform the ball
    const auto trans = getBallTransform(0.0f, 0.0f, ballRadius, 0.0f, objective, gradientX, gradientY);
    tm->setTransform(ball->getEntity(), trans);

    // Trace out the descent path
    auto tracer = DescentTracer::Builder()
            .objective(objective)
            .gradientX(gradientX)
            .gradientY(gradientY)
            .traceSize(0.1f)
            .heightPadding(0.01f)
            .usePhong(true)
            .traceMaterial(phong::COPPER)
            .markMaterial(phong::OBSIDIAN)
            .build();

    // The contour map
    const auto contour = Contour::Builder(objective)
            .low(-1.0f)
            .high(5.0f)
            .halfExtent(MESH_HALF_EXTENT)
            .segments(100)
            .build(*engine);

    const auto contourTracer = ContourTracer::Builder()
            .gradientX(gradientX)
            .gradientY(gradientY)
            .traceSize(0.1f)
            .heightPadding(0.01f)
            .traceColor(0.0f, 0.0f, 0.0f)
            .markColor(0.0f, 0.0f, 0.0f)
            .build();

    // The contour ball
    const auto contourBall = Sphere::SubdivisionBuilder()
            .uniformColor(0.0f, 0.0f, 0.0f)
            .initialPolygon(Sphere::SubdivisionBuilder::Polyhedron::ICOSAHEDRON)
            .build(*engine);
    constexpr auto contourBallRadius = 0.4f;
    tm->setTransform(contourBall->getEntity(), getContourBallTransform(0.0f, 0.0f, contourBallRadius));

    // Random the SGD state on R press
    context->setOnPress(Context::Key::R, [&]{
        // Random the position of the ball on the mesh
        sgd->randomState(8.0f);
        // Get the random position
        const auto [x, y] = sgd->getState();
        // Reset the rolling angle first
        ballAngle = 0.0f;
        // Transform the ball
        const auto trans = getBallTransform(x, y, ballRadius, 0.0f, objective, gradientX, gradientY);
        tm->setTransform(ball->getEntity(), trans);
        // Transform the contour ball
        const auto contourTrans = getContourBallTransform(x, y, contourBallRadius);
        tm->setTransform(contourBall->getEntity(), contourTrans);
        // Reset the tracers
        tracer->resetTo(x, y, *scene, *engine);
        contourTracer->resetTo(x, y, *contourScene, *engine);
    });

    // Move the ball to some interested position with Z
    context->setOnPress(Context::Key::Z, [&]{
        const auto x = 5.5f;
        const auto y = 0.0f;
        // Move the ball to some peak of the mesh
        sgd->resetState(x, y);
        // Reset the rolling angle first
        ballAngle = 0.0f;
        // Transform the ball
        const auto trans = getBallTransform(x, y, ballRadius, 0.0f, objective, gradientX, gradientY);
        tm->setTransform(ball->getEntity(), trans);
        // Transform the contour ball
        const auto contourTrans = getContourBallTransform(x, y, contourBallRadius);
        tm->setTransform(contourBall->getEntity(), contourTrans);
        // Reset the tracer
        tracer->resetTo(x, y, *scene, *engine);
        contourTracer->resetTo(x, y, *contourScene, *engine);
    });

    // Move the ball to the position with mouse click on the contour map
    Context::setOnMouseLeftClick([&](const auto xPos, const auto yPos) {
        const auto& [width, height] = context->getFramebufferSize();
        const auto phi = contourCam->getLongitudeAngle();
        float x, y;
        if (isMouseClickInContour(
            xPos, yPos, width, height,
            MESH_HALF_EXTENT, HEAT_VIEW_MIN_HE, phi, -90.0f, &x, &y
        )) {
            // Move the ball to this position
            sgd->resetState(x, y);
            // Reset the rolling angle first
            ballAngle = 0.0f;
            // Transform the ball
            const auto trans = getBallTransform(x, y, ballRadius, 0.0f, objective, gradientX, gradientY);
            tm->setTransform(ball->getEntity(), trans);
            // Transform the contour ball
            const auto contourTrans = getContourBallTransform(x, y, contourBallRadius);
            tm->setTransform(contourBall->getEntity(), contourTrans);
            // Reset the tracer
            tracer->resetTo(x, y, *scene, *engine);
            contourTracer->resetTo(x, y, *contourScene, *engine);
        }
    });

    // Manually descent the ball on SPACE holding
    context->setOnLongPress(Context::Key::SPACE, [&]{
        // Get the current position
        const auto [prevX, prevY] = sgd->getState();
        // Descent the ball
        sgd->iterate(MESH_HALF_EXTENT);
        // Get the new position
        const auto [x, y] = sgd->getState();
        const auto distance = glm::distance(glm::vec2{ prevX, prevY }, glm::vec2{ x, y });
        // Transform the ball
        const auto trans = getBallTransform(x, y, ballRadius, distance, objective, gradientX, gradientY);
        tm->setTransform(ball->getEntity(), trans);
        // Transform the contour ball
        const auto contourTrans = getContourBallTransform(x, y, contourBallRadius);
        tm->setTransform(contourBall->getEntity(), contourTrans);
        // Make traces
        tracer->traceTo(x, y, *scene, *engine);
        contourTracer->traceTo(x, y, *contourScene, *engine);
    });

    // Add a global light
    const auto globalLight = EntityManager::get()->create();
    LightManager::Builder(LightManager::Type::DIRECTIONAL)
            .direction(1.0f, 0.5f, -0.5f)
            .ambient(0.1f, 0.1f, 0.1f)
            .build(globalLight);

    // Render a small aura
    const auto auraPos = glm::vec3{8.0f, 8.0f, 8.0f };
    const auto pointLight = EntityManager::get()->create();
    LightManager::Builder(LightManager::Type::POINT)
            .position(auraPos.x, auraPos.y, auraPos.z)
            .build(pointLight);
    const auto aura = Aura::Builder().build(*engine);
    const auto auraTrans = translate(glm::mat4(1.0f), auraPos);
    tm->setTransform(aura->getEntity(), auraTrans);

    // Add renderables to the scene
    scene->addEntity(ball->getEntity());
    scene->addEntity(mesh->getEntity());
    scene->addEntity(aura->getEntity());
    scene->addEntity(pointLight);
    scene->addEntity(globalLight);

    contourScene->addEntity(contour->getEntity());
    contourScene->addEntity(contourBall->getEntity());

    // The render loop
    context->loop([&] {
        renderer->render(*view);
        renderer->render(*contourView);
    });

    // Destroy all resources
    engine->destroyEntity(ball->getEntity());
    engine->destroyEntity(contourBall->getEntity());
    engine->destroyEntity(mesh->getEntity());
    engine->destroyEntity(aura->getEntity());
    engine->destroyEntity(globalLight);
    engine->destroyEntity(pointLight);
    engine->destroyTexture(ballDiff);
    engine->destroyTexture(ballSpec);
    engine->destroyShader(ball->getShader());
    engine->destroyShader(contourBall->getShader());
    engine->destroyShader(mesh->getShader());
    engine->destroyShader(aura->getShader());

    engine->destroyRenderer(renderer);
    engine->destroyView(view);
    engine->destroyView(contourView);
    engine->destroySkybox(skybox);
    engine->destroySkybox(contourSkybox);
    engine->destroyScene(scene);
    engine->destroyScene(contourScene);
    engine->destroyCamera(camera->getEntity());

    const auto entityManager = EntityManager::get();
    entityManager->discard(ball->getEntity());
    entityManager->discard(contourBall->getEntity());
    entityManager->discard(mesh->getEntity());
    entityManager->discard(aura->getEntity());
    entityManager->discard(globalLight);
    entityManager->discard(pointLight);

    // Free up any resources we may have forgotten to destroy
    engine->destroy();

    return 0;
}

glm::mat4 getBallTransform(
    const float x, const float y, const float ballRadius, const float distance,
    const std::function<float(float, float)>& objective,
    const std::function<float(float, float)>& gradientX,
    const std::function<float(float, float)>& gradientY
) {
    const auto z = objective(x, y);
    // Get the normal vector at this position
    const auto dirX = gradientX(x, y);
    const auto dirY = gradientY(x, y);
    const auto norm = -glm::normalize(glm::vec3{ dirX, dirY, -1.0f });
    // Finally, translate the ball in the normal direction so that the ball will barely touch the mesh.
    auto trans = glm::translate(glm::mat4(1.0f), norm * ballRadius);
    // Then, we translated the ball to the new position.
    trans = glm::translate(trans, glm::vec3{ x, y, z });
    // Then, we rotate the ball according to the distance, updating the global angle (for the time being).
    ballAngle += distance / ballRadius;
    const auto rollDirection = glm::normalize(glm::vec3{ -dirX, -dirY, 0.0f });
    const auto rotationAxis = glm::cross(glm::vec3{ 0.0f, 0.0f, 1.0f }, rollDirection);
    const auto quaternion = glm::angleAxis(ballAngle, rotationAxis);
    const auto rotationMat = glm::mat4_cast(quaternion);
    trans *= rotationMat;
    // We first scale the ball uniformly to its radius.
    trans = glm::scale(trans, glm::vec3{ ballRadius, ballRadius, ballRadius });
    return trans;
}

glm::mat4 getContourBallTransform(const float x, const float y, const float radius) {
    auto trans = glm::translate(glm::mat4(1.0f), glm::vec3{ x, y, 0.0f });
    trans = glm::scale(trans, glm::vec3{ radius, radius, radius });
    return trans;
}

bool isMouseClickInContour(
    const float x, const float y, const int frameWidth, const int frameHeight,
    const float mapHalfExtent, const float viewMinHalfExtent, const float phi, const float initialPhi,
    float* const outX, float* const outY
) {
    const auto pivotX = static_cast<float>(frameWidth) * 3.0f / 4.0f;
    const auto pivotY = static_cast<float>(frameHeight) / 2.0f;

    const auto w = static_cast<float>(frameWidth);
    const auto h = static_cast<float>(frameHeight);
    const auto scaleFactor = ((w / 2.0f) > h) ? h / (viewMinHalfExtent * 2.0f) : w / (viewMinHalfExtent * 4.0f);

    const auto xLocal = (x - pivotX) / scaleFactor;
    const auto yLocal = (pivotY - y) / scaleFactor;

    const auto angleRadians = glm::radians(phi - initialPhi);
    const auto cosAngle = glm::cos(angleRadians);
    const auto sinAngle = glm::sin(angleRadians);

    const auto xPos = cosAngle * xLocal - sinAngle * yLocal;
    const auto yPos = sinAngle * xLocal + cosAngle * yLocal;

    if (std::abs(xPos) > mapHalfExtent || std::abs(yPos) > mapHalfExtent) {
        return false;
    }

    *outX = xPos;
    *outY = yPos;
    return true;
}
