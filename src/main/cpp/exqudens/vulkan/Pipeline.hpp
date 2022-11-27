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

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

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

    vk::raii::PipelineLayout& layoutReference();

    vk::raii::PipelineCache& cacheReference();

    vk::raii::Pipeline& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Pipeline::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::function<std::vector<char>(const std::string&)> readFileFunction;
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

      Pipeline::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      Pipeline::Builder& setReadFileFunction(const std::function<std::vector<char>(const std::string&)>& val);

      Pipeline::Builder& addSetLayout(const vk::DescriptorSetLayout& val);

      Pipeline::Builder& setSetLayouts(const std::vector<vk::DescriptorSetLayout>& val);

      Pipeline::Builder& addPushConstantRange(const vk::PushConstantRange& val);

      Pipeline::Builder& setPushConstantRanges(const std::vector<vk::PushConstantRange>& val);

      Pipeline::Builder& setLayoutCreateInfo(const vk::PipelineLayoutCreateInfo& val);

      Pipeline::Builder& setCacheCreateInfo(const vk::PipelineCacheCreateInfo& val);

      Pipeline::Builder& setComputeCreateInfo(const vk::ComputePipelineCreateInfo& val);

      Pipeline::Builder& setGraphicsCreateInfo(const GraphicsPipelineCreateInfo& val);

      Pipeline::Builder& setRayTracingCreateInfo(const vk::RayTracingPipelineCreateInfoNV& val);

      Pipeline::Builder& addStage(const vk::PipelineShaderStageCreateInfo& val);

      Pipeline::Builder& setStages(const std::vector<vk::PipelineShaderStageCreateInfo>& val);

      Pipeline::Builder& addPath(const std::string& val);

      Pipeline::Builder& setPaths(const std::vector<std::string>& val);

      Pipeline build();

  };

}
