#pragma once

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS //deprecated ig now uses radians as default
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Core.inl"
#include "GameObject.h"
#include "Window.h"
#include "VkSetup.h"
#include "Renderer.h"
#include "MovementControls_keyboard.h"
#include "Camera.h"
#include "SimpleRenderSystem.h"

#include <memory>
#include <vector>

namespace Nasha{
    class API Application{
    public:
        Application();
        virtual ~Application();

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

        void run();

    private:
        void loadGameObjects();

    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

    private:
        Window g_window{WIDTH, HEIGHT, "Nasha-dev"};
        VkSetup g_vkSetup{g_window};
        Renderer g_renderer{g_window, g_vkSetup};
        std::vector<GameObject> g_gameObjects{};
    };

    Application* createApplication();
}