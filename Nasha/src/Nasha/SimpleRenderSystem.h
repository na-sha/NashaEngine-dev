#pragma once

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS //deprecated -- now radians by default
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Camera.h"
#include "GameObject.h"
#include "VkSetup.h"
#include "Pipeline.h"
#include "FrameInfo.h"

#include <memory>
#include <vector>

namespace Nasha{
    class SimpleRenderSystem{
    public:
        SimpleRenderSystem(VkSetup& device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void renderGameObjects(FrameInfo& frameInfo,
                               std::vector<GameObject>& gameObjects);

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