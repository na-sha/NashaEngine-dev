#pragma once

#include "Camera.h"

#include <vulkan/vulkan.h>

namespace Nasha{
    struct FrameInfo{
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera& camera;
    };
}