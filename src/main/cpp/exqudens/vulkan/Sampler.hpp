#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT Sampler {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    vk::SamplerCreateInfo createInfo;
    std::shared_ptr<vk::raii::Sampler> value;

    vk::raii::Sampler& reference();

  };

  class EXQUDENS_VULKAN_EXPORT Sampler::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::SamplerCreateInfo> createInfo;

    public:

      Sampler::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      Sampler::Builder& setCreateInfo(const vk::SamplerCreateInfo& val);

      Sampler build();

  };

}
