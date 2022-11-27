#pragma once

#include <optional>
#include <memory>
#include <stdexcept>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/macros.hpp"

namespace exqudens::vulkan {

  struct Surface {

    class Builder;

    inline static Builder builder();

    std::shared_ptr<vk::raii::SurfaceKHR> value;

    vk::raii::SurfaceKHR& reference() {
      try {
        if (!value) {
          throw std::runtime_error(CALL_INFO() + ": value is not initialized!");
        }
        return *value;
      } catch (...) {
        std::throw_with_nested(std::runtime_error(CALL_INFO()));
      }
    }

  };

  class Surface::Builder {

    private:

      std::weak_ptr<vk::raii::Instance> instance;
      VkSurfaceKHR vkSurface;

    public:

      Surface::Builder& setInstance(const std::weak_ptr<vk::raii::Instance>& val) {
        instance = val;
        return *this;
      }

      Surface::Builder& setVkSurface(const VkSurfaceKHR& val) {
        vkSurface = val;
        return *this;
      }

      Surface build() {
        try {
          Surface target = {};
          target.value = std::make_shared<vk::raii::SurfaceKHR>(
              *instance.lock(),
              vkSurface
          );
          return target;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

  };

  Surface::Builder Surface::builder() {
    return {};
  }

}
