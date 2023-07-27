#pragma once

#include "VkSetup.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Nasha{
    class Model{
    public:
        struct Vertex{
            glm::vec3 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };
        struct Builder{
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
        };

        Model(VkSetup& device, const Model::Builder& builder);
        ~Model();

        Model(const Model&) = delete;
        Model &operator=(const Model&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer) const;
    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);
    public:
    private:
        VkSetup& m_device;

        VkBuffer m_vertexBuffer{};
        VkDeviceMemory m_vertexBufferMemory{};
        uint32_t m_vertexCount{};

        bool hasIndexBuffer{false};
        VkBuffer m_indexBuffer{};
        VkDeviceMemory m_indexBufferMemory{};
        uint32_t m_indexCount{};
    };
}