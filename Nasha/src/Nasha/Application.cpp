#include "Application.h"

namespace Nasha{
    struct SimplePushConstantData{
        glm::mat2 transform{1.0f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    Application::Application() {
        loadGameObjects();
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

    void Application::loadGameObjects() {
        std::vector<Model::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
        auto lveModel = std::make_shared<Model>(g_vkSetup, vertices);

        auto triangle = GameObject::createGameObject();
        triangle.model = lveModel;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2D.translation.x = .2f;
        triangle.transform2D.scale = {2.f, .5f};
        triangle.transform2D.rotation = .25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

    void Application::createPipelineLayout() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
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

        renderGameObjects(g_commandBuffers[imageIndex]);

        vkCmdEndRenderPass(g_commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(g_commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void Application::renderGameObjects(VkCommandBuffer commandBuffer) {
        g_pipeline->bind(commandBuffer);

        for (auto &obj: gameObjects) {
            SimplePushConstantData push{};
            push.offset = obj.transform2D.translation;
            push.color = obj.color;
            push.transform = obj.transform2D.mat2();

            vkCmdPushConstants(
                    commandBuffer,
                    g_pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(SimplePushConstantData),
                    &push);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
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
