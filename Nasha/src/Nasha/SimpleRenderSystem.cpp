#include "SimpleRenderSystem.h"

namespace Nasha{
    struct SimplePushConstantData{
        glm::mat2 transform{1.0f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    SimpleRenderSystem::SimpleRenderSystem(VkSetup& device, VkRenderPass renderPass) : m_device{device} {
        createPipelineLayout();
        createPipeline(renderPass);
    };

    SimpleRenderSystem::~SimpleRenderSystem(){
        vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
    };

    void SimpleRenderSystem::createPipelineLayout() {
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
        if (vkCreatePipelineLayout(m_device.device(),
                                   &pipelineLayoutInfo,
                                   nullptr,
                                   &m_pipelineLayout) !=VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        m_pipeline = std::make_unique<Pipeline>(m_device,
                                                "../Nasha/src/Nasha/shaders/SimpleShader.vert.spv",
                                                "../Nasha/src/Nasha/shaders/SimpleShader.frag.spv",
                                                pipelineConfig);
    }

    void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects) {
        m_pipeline->bind(commandBuffer);

        for (auto &obj: gameObjects) {
            SimplePushConstantData push{};
            push.offset = obj.transform2D.translation;
            push.color = obj.color;
            push.transform = obj.transform2D.mat2();

            vkCmdPushConstants(
                    commandBuffer,
                    m_pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(SimplePushConstantData),
                    &push);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }
}
