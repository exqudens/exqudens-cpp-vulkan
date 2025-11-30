#include "exqudens/vulkan/WriteDescriptorSet.hpp"

namespace exqudens::vulkan {

  WriteDescriptorSet& WriteDescriptorSet::setDstSet(const vk::DescriptorSet& val) {
    vk::WriteDescriptorSet::setDstSet(val);
    return *this;
  }

  WriteDescriptorSet& WriteDescriptorSet::setDstBinding(const uint32_t& val) {
    vk::WriteDescriptorSet::setDstBinding(val);
    return *this;
  }

  WriteDescriptorSet& WriteDescriptorSet::setDstArrayElement(const uint32_t& val) {
    vk::WriteDescriptorSet::setDstArrayElement(val);
    return *this;
  }

  WriteDescriptorSet& WriteDescriptorSet::setDescriptorCount(const uint32_t& val) {
    vk::WriteDescriptorSet::setDescriptorCount(val);
    return *this;
  }

  WriteDescriptorSet& WriteDescriptorSet::setDescriptorType(const vk::DescriptorType& val) {
    vk::WriteDescriptorSet::setDescriptorType(val);
    return *this;
  }

  WriteDescriptorSet& WriteDescriptorSet::addImageInfo(const vk::DescriptorImageInfo& val) {
    imageInfo.emplace_back(val);
    setImageInfo(imageInfo);
    return *this;
  }

  WriteDescriptorSet& WriteDescriptorSet::setImageInfo(const std::vector<vk::DescriptorImageInfo>& val) {
    imageInfo = val;
    vk::WriteDescriptorSet::setImageInfo(imageInfo);
    return *this;
  }

  WriteDescriptorSet& WriteDescriptorSet::addBufferInfo(const vk::DescriptorBufferInfo& val) {
    bufferInfo.emplace_back(val);
    setBufferInfo(bufferInfo);
    return *this;
  }

  WriteDescriptorSet& WriteDescriptorSet::setBufferInfo(const std::vector<vk::DescriptorBufferInfo>& val) {
    bufferInfo = val;
    vk::WriteDescriptorSet::setBufferInfo(bufferInfo);
    return *this;
  }

  WriteDescriptorSet& WriteDescriptorSet::addTexelBufferView(const vk::BufferView& val) {
    texelBufferView.emplace_back(val);
    setTexelBufferView(texelBufferView);
    return *this;
  }

  WriteDescriptorSet& WriteDescriptorSet::setTexelBufferView(const std::vector<vk::BufferView>& val) {
    texelBufferView = val;
    vk::WriteDescriptorSet::setTexelBufferView(texelBufferView);
    return *this;
  }

}
