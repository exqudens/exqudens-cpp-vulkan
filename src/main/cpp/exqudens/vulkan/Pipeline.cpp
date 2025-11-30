#include <map>
#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Pipeline.hpp"
#include "exqudens/vulkan/Utility.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::PipelineLayout& Pipeline::layoutReference() {
    try {
      if (!layout) {
        throw std::runtime_error(CALL_INFO + ": layout is not initialized!");
      }
      return *layout;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  vk::raii::PipelineCache& Pipeline::cacheReference() {
    try {
      if (!cache) {
        throw std::runtime_error(CALL_INFO + ": cache is not initialized!");
      }
      return *cache;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  vk::raii::Pipeline& Pipeline::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Pipeline::Builder& Pipeline::Builder::setDevice(const std::weak_ptr<vk::raii::Device>& val) {
    device = val;
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::setReadFileBytesFunction(const std::function<std::vector<char>(const std::string&)>& val) {
    readFileBytesFunction = val;
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::addSetLayout(const vk::DescriptorSetLayout& val) {
    setLayouts.emplace_back(val);
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::setSetLayouts(const std::vector<vk::DescriptorSetLayout>& val) {
    setLayouts = val;
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::addPushConstantRange(const vk::PushConstantRange& val) {
    pushConstantRanges.emplace_back(val);
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::setPushConstantRanges(const std::vector<vk::PushConstantRange>& val) {
    pushConstantRanges = val;
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::setLayoutCreateInfo(const vk::PipelineLayoutCreateInfo& val) {
    layoutCreateInfo = val;
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::setCacheCreateInfo(const vk::PipelineCacheCreateInfo& val) {
    cacheCreateInfo = val;
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::setComputeCreateInfo(const vk::ComputePipelineCreateInfo& val) {
    computeCreateInfo = val;
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::setGraphicsCreateInfo(const exqudens::vulkan::GraphicsPipelineCreateInfo& val) {
    graphicsCreateInfo = val;
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::setRayTracingCreateInfo(const vk::RayTracingPipelineCreateInfoNV& val) {
    rayTracingCreateInfo = val;
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::addStage(const vk::PipelineShaderStageCreateInfo& val) {
    stages.emplace_back(val);
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::setStages(const std::vector<vk::PipelineShaderStageCreateInfo>& val) {
    stages = val;
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::addPath(const std::string& val) {
    paths.emplace_back(val);
    return *this;
  }

  Pipeline::Builder& Pipeline::Builder::setPaths(const std::vector<std::string>& val) {
    paths = val;
    return *this;
  }

  Pipeline Pipeline::Builder::build() {
    try {
      if (!readFileBytesFunction) {
        readFileBytesFunction = &Utility::readFileBytes;
      }

      Pipeline target = {};
      target.setLayouts = setLayouts;
      target.pushConstantRanges = pushConstantRanges;
      target.layoutCreateInfo = layoutCreateInfo.value_or(vk::PipelineLayoutCreateInfo());
      target.layoutCreateInfo.setSetLayouts(target.setLayouts);
      target.layoutCreateInfo.setPushConstantRanges(target.pushConstantRanges);
      target.layout = std::make_shared<vk::raii::PipelineLayout>(
          *device.lock(),
          target.layoutCreateInfo
      );
      target.cacheCreateInfo = cacheCreateInfo.value_or(vk::PipelineCacheCreateInfo());
      target.cache = std::make_shared<vk::raii::PipelineCache>(
          *device.lock(),
          target.cacheCreateInfo
      );
      target.computeCreateInfo = computeCreateInfo;
      target.graphicsCreateInfo = graphicsCreateInfo;
      target.rayTracingCreateInfo = rayTracingCreateInfo;
      std::map<std::string, std::pair<vk::ShaderModuleCreateInfo, std::shared_ptr<vk::raii::ShaderModule>>> shaders;
      if (graphicsCreateInfo) {
        for (const std::string& path : paths) {
          if (!shaders.contains(path)) {
            std::vector<char> bytes = readFileBytesFunction(path);
            if (bytes.empty()) {
              throw std::runtime_error(CALL_INFO + ": '" + path + "' failed to create shader module bytes is empty!");
            }
            vk::ShaderModuleCreateInfo shaderCreateInfo = vk::ShaderModuleCreateInfo()
                .setCodeSize(bytes.size())
                .setPCode(reinterpret_cast<const uint32_t*>(bytes.data()));
            shaders[path] = std::make_pair(
                shaderCreateInfo,
                std::make_shared<vk::raii::ShaderModule>(*device.lock(), shaderCreateInfo)
            );
            vk::PipelineShaderStageCreateInfo stage = vk::PipelineShaderStageCreateInfo();
            stage.setPName("main");
            stage.setModule(*(*shaders[path].second));
            if (path.ends_with(".vert.spv")) {
              stage.setStage(vk::ShaderStageFlagBits::eVertex);
            } else if (path.ends_with(".frag.spv")) {
              stage.setStage(vk::ShaderStageFlagBits::eFragment);
            } else {
              throw std::invalid_argument(CALL_INFO + ": '" + path + "' failed to create shader!");
            }
            stages.emplace_back(stage);
          }
        }
        target.stages = stages;
        target.graphicsCreateInfo.value().setStages(target.stages);
        target.graphicsCreateInfo.value().setLayout(*target.layoutReference());
        target.value = std::make_shared<vk::raii::Pipeline>(
            *device.lock(),
            target.cacheReference(),
            target.graphicsCreateInfo.value()
        );
      }
      return target;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Pipeline::Builder Pipeline::builder() {
    return {};
  }

}

#undef CALL_INFO
