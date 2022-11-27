#pragma once

#include <optional>
#include <vector>
#include <memory>
#include <stdexcept>

#include <vulkan/vulkan_raii.hpp>

#include "exqudens/vulkan/macros.hpp"
#include "exqudens/vulkan/WriteDescriptorSet.hpp"

namespace exqudens::vulkan {

  struct DescriptorSet {

    class Builder;

    inline static Builder builder();

    std::vector<vk::DescriptorSetLayout> setLayouts;
    vk::DescriptorSetAllocateInfo createInfo;
    std::vector<WriteDescriptorSet> writes;
    std::shared_ptr<vk::raii::DescriptorSet> value;

    vk::raii::DescriptorSet& reference() {
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

  class DescriptorSet::Builder {

    private:

      std::weak_ptr<vk::raii::Device> device;
      std::vector<vk::DescriptorSetLayout> setLayouts;
      std::optional<vk::DescriptorSetAllocateInfo> createInfo;
      std::vector<WriteDescriptorSet> writes;

    public:

      DescriptorSet::Builder& setDevice(const std::weak_ptr<vk::raii::Device>& val) {
        device = val;
        return *this;
      }

      DescriptorSet::Builder& addSetLayout(const vk::DescriptorSetLayout& val) {
        setLayouts.emplace_back(val);
        return *this;
      }

      DescriptorSet::Builder& setSetLayouts(const std::vector<vk::DescriptorSetLayout>& val) {
        setLayouts = val;
        return *this;
      }

      DescriptorSet::Builder& setCreateInfo(const vk::DescriptorSetAllocateInfo& val) {
        createInfo = val;
        return *this;
      }

      DescriptorSet::Builder& setWrites(const std::vector<WriteDescriptorSet>& val) {
        writes = val;
        return *this;
      }

      DescriptorSet build() {
        try {
          DescriptorSet target = {};
          target.setLayouts = setLayouts;
          target.createInfo = createInfo.value();
          target.createInfo.setSetLayouts(target.setLayouts);
          vk::raii::DescriptorSets values = vk::raii::DescriptorSets(
              *device.lock(),
              target.createInfo
          );
          target.value = std::make_shared<vk::raii::DescriptorSet>(std::move(values.front()));
          target.writes = writes;
          std::vector<vk::WriteDescriptorSet> tmpWrites;
          for (WriteDescriptorSet& write : target.writes) {
            write.setDstSet(*target.reference());
            tmpWrites.emplace_back(write);
          }
          device.lock()->updateDescriptorSets(tmpWrites, {});
          return target;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

  };

  DescriptorSet::Builder DescriptorSet::builder() {
    return {};
  }

}
