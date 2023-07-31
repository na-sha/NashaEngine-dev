#include "PointLightSystem.h"

namespace Nasha{
    PointLightSystem::PointLightSystem(
            VkSetup& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
            : lveDevice{device} {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem() {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        // VkPushConstantRange pushConstantRange{};
        // pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        // pushConstantRange.offset = 0;
        // pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(lveDevice.device(),
                                   &pipelineLayoutInfo,
                                   nullptr,
                                   &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void PointLightSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.attributeDescription.clear();
        pipelineConfig.bindingDescription.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<Pipeline>(
                lveDevice,
                "../Nasha/src/Nasha/shaders/PointLight.vert.spv",
                "../Nasha/src/Nasha/shaders/PointLight.frag.spv",
                pipelineConfig);
    }

    void PointLightSystem::render(FrameInfo& frameInfo) {
        lvePipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                0,
                1,
                &frameInfo.globalDescriptorSet,
                0,
                nullptr);

        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
}
