#pragma once

#include <vector>
#include <cstddef>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace exqudens::vulkan {

  struct Vertex {

    inline static vk::VertexInputBindingDescription getBindingDescription() {
      return vk::VertexInputBindingDescription()
          .setBinding(0)
          .setStride(sizeof(Vertex))
          .setInputRate(vk::VertexInputRate::eVertex);
    }

    inline static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
      uint32_t positionOffset = offsetof(Vertex, position);
      uint32_t colorOffset = offsetof(Vertex, color);
      uint32_t normalOffset = offsetof(Vertex, normal);
      uint32_t texCoordOffset = offsetof(Vertex, texCoord);
      return {
          vk::VertexInputAttributeDescription()
              .setBinding(0)
              .setLocation(0)
              .setFormat(vk::Format::eR32G32B32Sfloat)
              .setOffset(positionOffset),
          vk::VertexInputAttributeDescription()
              .setBinding(0)
              .setLocation(1)
              .setFormat(vk::Format::eR32G32B32Sfloat)
              .setOffset(colorOffset),
          vk::VertexInputAttributeDescription()
              .setBinding(0)
              .setLocation(2)
              .setFormat(vk::Format::eR32G32B32Sfloat)
              .setOffset(normalOffset),
          vk::VertexInputAttributeDescription()
              .setBinding(0)
              .setLocation(3)
              .setFormat(vk::Format::eR32G32Sfloat)
              .setOffset(texCoordOffset)
      };
    }

    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texCoord;

    bool operator==(const Vertex& other) const {
      return position == other.position && color == other.color && normal == other.normal && texCoord == other.texCoord;
    }

    template <typename T, typename... ARGS>
    inline static void hashCombine(std::size_t &seed, const T &v, ARGS &&... rest) {
      seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      (hashCombine(seed, rest), ...);
    }

  };

}

namespace std {
  template<> struct hash<exqudens::vulkan::Vertex> {
    size_t operator()(exqudens::vulkan::Vertex const& vertex) const {
      size_t result = 0;
      exqudens::vulkan::Vertex::hashCombine(
          result,
          hash<glm::vec3>()(vertex.position),
          hash<glm::vec3>()(vertex.color),
          hash<glm::vec2>()(vertex.normal),
          hash<glm::vec2>()(vertex.texCoord)
      );
      return result;
    }
  };
}
