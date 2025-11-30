#pragma once

#include <string>
#include <optional>
#include <vector>
#include <functional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"
#include "exqudens/vulkan/GraphicsPipelineCreateInfo.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Pipeline {

    class Builder;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES static Builder builder();

    std::vector<vk::DescriptorSetLayout> setLayouts;
    std::vector<vk::PushConstantRange> pushConstantRanges;
    vk::PipelineLayoutCreateInfo layoutCreateInfo;
    std::shared_ptr<vk::raii::PipelineLayout> layout;

    vk::PipelineCacheCreateInfo cacheCreateInfo;
    std::shared_ptr<vk::raii::PipelineCache> cache;

    std::optional<vk::ComputePipelineCreateInfo> computeCreateInfo;
    std::optional<GraphicsPipelineCreateInfo> graphicsCreateInfo;
    std::optional<vk::RayTracingPipelineCreateInfoNV> rayTracingCreateInfo;
    std::vector<vk::PipelineShaderStageCreateInfo> stages;
    std::shared_ptr<vk::raii::Pipeline> value;

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::PipelineLayout& layoutReference();

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::PipelineCache& cacheReference();

    EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES vk::raii::Pipeline& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Pipeline::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::function<std::vector<char>(const std::string&)> readFileBytesFunction;
      std::vector<vk::DescriptorSetLayout> setLayouts;
      std::vector<vk::PushConstantRange> pushConstantRanges;
      std::optional<vk::PipelineLayoutCreateInfo> layoutCreateInfo;
      std::optional<vk::PipelineCacheCreateInfo> cacheCreateInfo;
      std::optional<vk::ComputePipelineCreateInfo> computeCreateInfo;
      std::optional<GraphicsPipelineCreateInfo> graphicsCreateInfo;
      std::optional<vk::RayTracingPipelineCreateInfoNV> rayTracingCreateInfo;
      std::vector<vk::PipelineShaderStageCreateInfo> stages;
      std::vector<std::string> paths;

    public:

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& setReadFileBytesFunction(const std::function<std::vector<char>(const std::string&)>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& addSetLayout(const vk::DescriptorSetLayout& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& setSetLayouts(const std::vector<vk::DescriptorSetLayout>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& addPushConstantRange(const vk::PushConstantRange& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& setPushConstantRanges(const std::vector<vk::PushConstantRange>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& setLayoutCreateInfo(const vk::PipelineLayoutCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& setCacheCreateInfo(const vk::PipelineCacheCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& setComputeCreateInfo(const vk::ComputePipelineCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& setGraphicsCreateInfo(const GraphicsPipelineCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& setRayTracingCreateInfo(const vk::RayTracingPipelineCreateInfoNV& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& addStage(const vk::PipelineShaderStageCreateInfo& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& setStages(const std::vector<vk::PipelineShaderStageCreateInfo>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& addPath(const std::string& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline::Builder& setPaths(const std::vector<std::string>& val);

      EXQUDENS_VULKAN_FUNCTION_ATTRIBUTES Pipeline build();

  };

}
