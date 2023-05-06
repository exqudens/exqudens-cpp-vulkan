#pragma once

#include <vector>

#include "exqudens/vulkan/all.hpp"
#include "exqudens/vulkan/Context.hpp"
#include "exqudens/vulkan/Vertex.hpp"
#include "exqudens/vulkan/UniformBufferObject.hpp"

namespace exqudens::vulkan {

  struct DataContext {

    Buffer vertexStagingBuffer = {};
    Buffer vertexBuffer = {};
    Buffer indexStagingBuffer = {};
    Buffer indexBuffer = {};
    Buffer textureBuffer = {};
    Image textureImage = {};
    ImageView textureImageView = {};
    std::vector<Buffer> shadowUniformBuffers = {};
    std::vector<Buffer> uniformBuffers = {};

    void init(
        Context& root,
        std::vector<Vertex>& vertexVector,
        std::vector<uint16_t>& indexVector,
        uint32_t& textureWidth,
        uint32_t& textureHeight,
        uint32_t& textureDepth,
        uint32_t textureMipLevels
    );

  };

}
