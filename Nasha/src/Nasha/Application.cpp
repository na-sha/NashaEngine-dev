#include "Application.h"

namespace Nasha{
    Application::Application() {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    };
    Application::~Application(){
        vkDestroyPipelineLayout(g_vkSetup.device(), g_pipelineLayout, nullptr);
    };

    void Application::run() {
        while (!g_window.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }
        vkDeviceWaitIdle(g_vkSetup.device());
    }

    void Application::loadModels() {
        std::vector<Model::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
        g_model = std::make_unique<Model>(g_vkSetup, vertices);
    }

    void Application::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(g_vkSetup.device(),
                                   &pipelineLayoutInfo,
                                   nullptr,
                                   &g_pipelineLayout) !=VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void Application::createPipeline() {
        assert(g_vkSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(g_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = g_vkSwapChain ->getRenderPass();
        pipelineConfig.pipelineLayout = g_pipelineLayout;
        g_pipeline = std::make_unique<Pipeline>(
                g_vkSetup,
                "../Nasha/src/Nasha/shaders/SimpleShader.vert.spv",
                "../Nasha/src/Nasha/shaders/SimpleShader.frag.spv",
                pipelineConfig);
    }

    void Application::recreateSwapChain() {
        VkExtent2D extent = g_window.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = g_window.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(g_vkSetup.device());

        if (g_vkSwapChain == nullptr) {
            g_vkSwapChain = std::make_unique<SwapChain>(g_vkSetup, extent);
        }else{
            g_vkSwapChain = std::make_unique<SwapChain>(g_vkSetup, extent, std::move(g_vkSwapChain));
            if (g_vkSwapChain -> imageCount() != g_commandBuffers.size()){
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        createPipeline();
    }

    void Application::createCommandBuffers() {
        g_commandBuffers.resize(g_vkSwapChain ->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = g_vkSetup.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(g_commandBuffers.size());

        if (vkAllocateCommandBuffers(g_vkSetup.device(), &allocInfo, g_commandBuffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void Application:: freeCommandBuffers(){
        vkFreeCommandBuffers(g_vkSetup.device(),
                             g_vkSetup.getCommandPool(),
                             static_cast<uint32_t>(g_commandBuffers.size()),
                             g_commandBuffers.data());
        g_commandBuffers.clear();
    }

    void Application::recordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(g_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = g_vkSwapChain ->getRenderPass();
        renderPassInfo.framebuffer = g_vkSwapChain ->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = g_vkSwapChain ->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(g_commandBuffers[imageIndex],
                             &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(g_vkSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(g_vkSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, g_vkSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(g_commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(g_commandBuffers[imageIndex], 0, 1, &scissor);

        g_pipeline->bind(g_commandBuffers[imageIndex]);
        g_model->bind(g_commandBuffers[imageIndex]);
        g_model->draw(g_commandBuffers[imageIndex]);

        vkCmdEndRenderPass(g_commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(g_commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void Application::drawFrame() {
        uint32_t imageIndex;
        auto result = g_vkSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        recordCommandBuffer(static_cast<int>(imageIndex));
        result = g_vkSwapChain->submitCommandBuffers(&g_commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
            g_window.wasWindowResized()) {
            g_window.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }
}
