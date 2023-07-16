#pragma once

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Core.inl"
#include "GameObject.h"
#include "Window.h"
#include "VkSetup.h"
#include "Pipeline.h"
#include "Swapchain.h"

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
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);
        void renderGameObjects(VkCommandBuffer commandBuffer);

    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

    private:
        Window g_window{WIDTH, HEIGHT, "Nasha-dev"};
        VkSetup g_vkSetup{g_window};
        std::unique_ptr<SwapChain> g_vkSwapChain;
        std::unique_ptr<Pipeline> g_pipeline{};
        VkPipelineLayout g_pipelineLayout{};
        std::vector<VkCommandBuffer> g_commandBuffers{};
        std::vector<GameObject> gameObjects{};
    };

    Application* createApplication();
}