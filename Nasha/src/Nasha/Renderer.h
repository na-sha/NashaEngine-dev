#pragma once

#include <GLFW/glfw3.h>

#include "Window.h"
#include "VkSetup.h"
#include "Swapchain.h"

#include <memory>
#include <vector>

namespace Nasha{
    class Renderer{
    public:
        Renderer(Window& window, VkSetup& device);
        ~Renderer();

        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;

        [[nodiscard]] VkRenderPass getSwapChainRenderPass() const { return m_swapChain->getRenderPass(); }
        [[nodiscard]] bool isFrameInProgress() const { return m_isFrameStarted; }

        [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const {
            assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress");
            return m_commandBuffers[m_currentFrameIndex];
        }

        [[nodiscard]] int getFrameIndex() const {
            assert(m_isFrameStarted && "Cannot get frame index when frame not in progress");
            return m_currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

    public:

    private:
        Window& m_window;
        VkSetup& m_device;
        std::unique_ptr<SwapChain> m_swapChain;
        std::vector<VkCommandBuffer> m_commandBuffers{};

        uint32_t m_currentImageIndex{};
        int m_currentFrameIndex{0};
        bool m_isFrameStarted{false};
    };
}