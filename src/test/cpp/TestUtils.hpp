#pragma once

#include <cstddef>
#include <string>
#include <optional>
#include <vector>
#include <map>
#include <chrono>
#include <functional>

#include "exqudens/vulkan/all.hpp"
#include "exqudens/vulkan/Vertex.hpp"

class TestUtils {

  public:

    template <typename T, typename... ARGS>
    inline static void hashCombine(std::size_t &seed, const T &v, ARGS &&... rest) {
      seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      (hashCombine(seed, rest), ...);
    }

    static std::string getExecutableFile();

    static std::string getExecutableDir();

    static std::optional<std::string> getEnvironmentVariable(const std::string& name);

    static void setEnvironmentVariable(const std::string& name, const std::string& value);

    static std::vector<std::string> toStringVector(
        const std::exception& exception,
        std::vector<std::string> previous = {}
    );

    static std::vector<std::string> toStackTrace(const std::exception& exception);

    static std::string toString(const std::exception& e);

    static void readPng(
        const std::string& path,
        unsigned int& width,
        unsigned int& height,
        unsigned int& depth,
        std::vector<unsigned char>& data
    );

    static std::vector<std::vector<std::vector<unsigned char>>> readPng(const std::string& path);

    static void writePng(
        const std::string& path,
        unsigned int& width,
        unsigned int& height,
        unsigned int& depth,
        std::vector<unsigned char>& data
    );

    static void writePng(const std::vector<std::vector<std::vector<unsigned char>>>& image, const std::string& path);

    static void readObj(
        const std::string& path,
        std::vector<exqudens::vulkan::Vertex>& vertices,
        std::vector<uint16_t>& indices
    );

    static vk::PipelineShaderStageCreateInfo createStage(
        exqudens::vulkan::Device& device,
        std::map<std::string, std::pair<vk::ShaderModuleCreateInfo, std::shared_ptr<vk::raii::ShaderModule>>>& shaders,
        const std::string& path
    );

    static void insertDepthImagePipelineBarrier(
        exqudens::vulkan::CommandBuffer& transferCommandBuffer,
        exqudens::vulkan::Image& depthImage
    );

    static void copyBufferToImageAndGenerateMipmaps(
        exqudens::vulkan::PhysicalDevice& physicalDevice,
        exqudens::vulkan::CommandBuffer& transferCommandBuffer,
        exqudens::vulkan::Buffer& textureBuffer,
        exqudens::vulkan::Image& textureImage
    );

    static void updateUniformBuffer(
        exqudens::vulkan::Buffer& shadowUniformBuffer,
        exqudens::vulkan::Buffer& uniformBuffer,
        std::chrono::time_point<std::chrono::high_resolution_clock>& startTime,
        float& angleLeft,
        float& angleUp,
        const uint32_t& width,
        const uint32_t& height,
        const std::string& animate,
        const bool& left,
        const bool& right,
        const bool& up,
        const bool& down
    );

    static void copySwapchainImageToOutputImage(
        exqudens::vulkan::CommandBuffer& transferCommandBuffer,
        exqudens::vulkan::Image& swapchainImage,
        exqudens::vulkan::Image& outputImage,
        exqudens::vulkan::Queue& transferQueue
    );

};
