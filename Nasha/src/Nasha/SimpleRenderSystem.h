#pragma once

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Camera.h"
#include "GameObject.h"
#include "VkSetup.h"
#include "Pipeline.h"

#include <memory>
#include <vector>

namespace Nasha{
    class SimpleRenderSystem{
    public:
        SimpleRenderSystem(VkSetup& device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void renderGameObjects(VkCommandBuffer commandBuffer,
                               std::vector<GameObject>& gameObjects,
                               const Camera& camera);

    private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

    public:

    private:
        VkSetup& m_device;
        std::unique_ptr<Pipeline> m_pipeline{};
        VkPipelineLayout m_pipelineLayout{};
    };
}