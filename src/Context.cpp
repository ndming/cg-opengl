// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

//
// Created by Minh Nguyen on 4/4/2023.
//

#include <glad/glad.h>
#include <chrono>
#include <functional>
#include <iostream>
#include <stdexcept>

#include "Context.h"

static std::function<void(int, int)> mFramebufferCallback{ [](auto, auto) {} };
static std::function<void(float, float)> mMouseLeftClickCallback{ [](auto, auto) {} };
static std::function<void(float)> mMouseScrollCallback{ [](auto) {} };
static std::function<void(float, float)> mMouseDragPerpetualCallback{ [](auto, auto) {} };

static bool mDragging = false;
static float mLastX = 0.0f;
static float mLastY = 0.0f;

std::unique_ptr<Context> Context::create(const std::string_view name, const int width, const int height) {
    return std::unique_ptr<Context>(new Context{ name, width, height });
}

Context::Context(const std::string_view name, const int width, const int height) {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    // ensure when a user does not have the proper OpenGL version GLFW fails to run
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VERSION_MINOR);
    // access to smaller subset of OpenGL without backward-compatible features
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    // required by most of GLFW's functions
    _window = glfwCreateWindow(width, height, name.data(), nullptr, nullptr);
    if (_window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window\n");
    }
    // make the window the main context on the current thread
    glfwMakeContextCurrent(_window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD\n";
        throw std::runtime_error("Failed to initialize GLAD\n");
    }

    initializeMouseCallbacks();
}

void Context::setClose(const bool close) const {
    glfwSetWindowShouldClose(_window, close);
}

Context::~Context() {
    glfwTerminate();
}

void Context::setOnPress(const Key key, const std::function<void()>& listener) {
    _onPressListeners.emplace(key, OnPressListener{ listener, false });
}

void Context::setOnLongPress(const Key key, const std::function<void()>& listener) {
    _onLongPressListeners[key] = listener;
}

void Context::initializeMouseCallbacks() const {
    glfwSetMouseButtonCallback(_window, [](const auto window, const auto button, const auto action, auto mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                double xPos, yPos;
                glfwGetCursorPos(window, &xPos, &yPos);

                mLastX = static_cast<float>(xPos);
                mLastY = static_cast<float>(yPos);
                mMouseLeftClickCallback(mLastX, mLastY);

                mDragging = true;
            }
            else if (action == GLFW_RELEASE) {
                mDragging = false;
            }
        }
    });

    glfwSetScrollCallback(_window, [](auto _, const auto offsetX, const auto offsetY) {
        mMouseScrollCallback(static_cast<float>(offsetY));
    });

    glfwSetCursorPosCallback(_window, [](auto _, const auto xPos, const auto yPos) {
        if (mDragging) {
            const auto offsetX = static_cast<float>(xPos) - mLastX;
            const auto offsetY = static_cast<float>(yPos) - mLastY;
            mLastX = static_cast<float>(xPos);
            mLastY = static_cast<float>(yPos);

            mMouseDragPerpetualCallback(offsetX, offsetY);
        }
    });
}

void Context::setOnMouseLeftClick(const std::function<void(float, float)> &callback) {
    mMouseLeftClickCallback = callback;
}

void Context::setOnMouseScroll(const std::function<void(float)>& callback) {
    mMouseScrollCallback = callback;
}

void Context::setOnMouseDragPerpetual(const std::function<void(float, float)>& callback) {
    mMouseDragPerpetualCallback = callback;
}

void Context::loop(const std::function<void()>& onFrame) {
    // We make sure the framebuffer callback gets invoked before the rendering loop.
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    mFramebufferCallback(width, height);

    while (!glfwWindowShouldClose(_window)) {
        static const auto START_POINT = std::chrono::high_resolution_clock::now();
        const auto currentPoint = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentPoint - START_POINT);
        _currentTime = std::chrono::duration<long, std::milli>(duration).count();
        _deltaTime = _currentTime - _lastTime;
        _lastTime = _currentTime;

        processInputListeners();

        onFrame();

        glfwPollEvents();
        glfwSwapBuffers(_window);
    }
}

long Context::getDeltaTimeMillis() const {
    return _deltaTime;
}

std::pair<int, int> Context::getFramebufferSize() const {
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    return std::make_pair(width, height);
}

std::pair<float, float> Context::getMousePos() const {
    double xPos, yPos;
    glfwGetCursorPos(_window, &xPos, &yPos);
    return { static_cast<float>(xPos), static_cast<float>(yPos) };
}

void Context::setFramebufferCallback(const std::function<void(int, int)>& callback) const {
    mFramebufferCallback = callback;
    glfwSetFramebufferSizeCallback(_window, [](auto _, const auto w, const auto h) {
        mFramebufferCallback(w, h);
    });
}

void Context::processInputListeners() {
    // For on-press listeners, we must guarantee the callback only get invoked once
    for (auto& [key, listener] : _onPressListeners) {
        if (glfwGetKey(_window, static_cast<int>(key)) == GLFW_PRESS && !listener.pressed) {
            listener.callback();
            listener.pressed = true;
        }
        else if (glfwGetKey(_window, static_cast<int>(key)) == GLFW_RELEASE && listener.pressed) {
            listener.pressed = false;
        }
    }

    for (const auto& [key, callback] : _onLongPressListeners) {
        if (glfwGetKey(_window, static_cast<int>(key)) == GLFW_PRESS) {
            callback();
        }
    }
}