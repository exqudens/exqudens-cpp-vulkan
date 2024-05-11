#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/Surface.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  vk::raii::SurfaceKHR& Surface::reference() {
    try {
      if (!value) {
        throw std::runtime_error(CALL_INFO + ": value is not initialized!");
      }
      return *value;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
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
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  Surface::Builder Surface::builder() {
    return {};
  }

}

#undef CALL_INFO
