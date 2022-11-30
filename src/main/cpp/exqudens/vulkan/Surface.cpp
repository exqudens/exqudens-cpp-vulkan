#include "exqudens/vulkan/Surface.hpp"
#include "exqudens/vulkan/macros.hpp"

#include <stdexcept>

namespace exqudens::vulkan {

  vk::raii::SurfaceKHR& Surface::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO() + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO()));
    }
  }

  Surface::Builder& Surface::Builder::setInstance(const std::weak_ptr<vk::raii::Instance>& val) {
    instance = val;
    return *this;
  }

  Surface::Builder& Surface::Builder::setVkSurface(const VkSurfaceKHR& val) {
    vkSurface = val;
    return *this;
  }

  Surface Surface::Builder::build() {
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

  Surface::Builder Surface::builder() {
    return {};
  }

}
