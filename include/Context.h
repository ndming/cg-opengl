// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glad/glad.h> // GLAD must be included before GLFW
#include <GLFW/glfw3.h>
#include <string_view>
#include <functional>
#include <map>
#include <memory>
#include <utility>

class Context {
public:
    ~Context();
    Context(const Context&) = delete;
    Context(Context&&) noexcept = delete;
    Context& operator=(const Context&) = delete;
    Context& operator=(Context&&) noexcept = delete;

    enum class Key {
        ESC = GLFW_KEY_ESCAPE,
        LCTR = GLFW_KEY_LEFT_CONTROL,
        LSHF = GLFW_KEY_LEFT_SHIFT,
        A = GLFW_KEY_A,
        C = GLFW_KEY_C,
        D = GLFW_KEY_D,
        F = GLFW_KEY_F,
        I = GLFW_KEY_I,
        R = GLFW_KEY_R,
        S = GLFW_KEY_S,
        T = GLFW_KEY_T,
        W = GLFW_KEY_W,
        SPACE = GLFW_KEY_SPACE
    };
    static std::unique_ptr<Context> create(
            std::string_view name = "Computer Graphics",
            int width = 800, int height = 600
    );

    void setClose(bool close) const;

    void setFramebufferCallback(const std::function<void(int, int)>& callback) const;

    void setOnPress(Key key, const std::function<void()>& listener);

    void setOnLongPress(Key key, const std::function<void()>& listener);

    void setMouseScrollCallback(const std::function<void(float)>& callback) const;

    void setMouseDragPerpetualCallback(const std::function<void(float, float)>& callback) const;

    void loop(const std::function<void()>& onFrame);

    [[nodiscard]] float getCurrentTime() const;

    [[nodiscard]] float getDeltaTime() const;

    [[nodiscard]] std::pair<int, int> getFramebufferSize() const;

    static constexpr auto VERSION_MAJOR = 4;
    static constexpr auto VERSION_MINOR = 4;

private:
    explicit Context(std::string_view name, int width, int height);

    GLFWwindow* _window;

    float _deltaTime{ 0.0f };
    float _lastTime{ 0.0f };
    float _currentTime{ 0.0f };

    struct OnPressListener {
        const std::function<void()> callback;
        bool pressed{ false };
    };

    std::map<Key, OnPressListener> _onPressListeners{};

    std::map<Key, std::function<void()>> _onLongPressListeners{};

    void processInputListeners();

    class Factory {
    public:
        std::unique_ptr<Context> operator()(std::string_view name, int width, int height) const;
    };
};
