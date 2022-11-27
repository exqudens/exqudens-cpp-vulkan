#pragma once

#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/export.hpp"

namespace exqudens::vulkan {

  struct EXQUDENS_VULKAN_EXPORT ImageView {

    class Builder;

    EXQUDENS_VULKAN_INTERFACE_INLINE static Builder builder();

    vk::ImageViewCreateInfo createInfo;
    std::shared_ptr<vk::raii::ImageView> value;

    vk::raii::ImageView& reference();

  };

  class EXQUDENS_VULKAN_EXPORT ImageView::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::optional<vk::ImageViewCreateInfo> createInfo;

    public:

      ImageView::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val);

      ImageView::Builder& setCreateInfo(const vk::ImageViewCreateInfo& val);

      ImageView build();

  };

}
