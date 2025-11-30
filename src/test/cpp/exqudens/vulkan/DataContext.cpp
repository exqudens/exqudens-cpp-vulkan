#include <functional>
#include <filesystem>
#include <stdexcept>

#include "exqudens/vulkan/DataContext.hpp"

#define CALL_INFO std::string(__FUNCTION__) + " (" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

namespace exqudens::vulkan {

  void DataContext::init(
      Context& root,
      std::vector<Vertex>& vertexVector,
      std::vector<uint16_t>& indexVector,
      uint32_t& textureWidth,
      uint32_t& textureHeight,
      uint32_t& textureDepth,
      uint32_t textureMipLevels
  ) {
    try {
      vertexStagingBuffer = Buffer::builder()
          .setPhysicalDevice(root.physicalDevice.value)
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::BufferCreateInfo()
                  .setSize(sizeof(vertexVector[0]) * vertexVector.size())
                  .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
                  .setSharingMode(vk::SharingMode::eExclusive)
          )
          .setMemoryCreateInfo(
              vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
          )
          .build();

      vertexBuffer = Buffer::builder()
          .setPhysicalDevice(root.physicalDevice.value)
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::BufferCreateInfo()
                  .setSize(vertexStagingBuffer.createInfo.size)
                  .setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer)
                  .setSharingMode(vk::SharingMode::eExclusive)
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
          .build();

      indexStagingBuffer = Buffer::builder()
          .setPhysicalDevice(root.physicalDevice.value)
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::BufferCreateInfo()
                  .setSize(sizeof(indexVector[0]) * indexVector.size())
                  .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
                  .setSharingMode(vk::SharingMode::eExclusive)
          )
          .setMemoryCreateInfo(
              vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
          )
          .build();

      indexBuffer = Buffer::builder()
          .setPhysicalDevice(root.physicalDevice.value)
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::BufferCreateInfo()
                  .setSize(indexStagingBuffer.createInfo.size)
                  .setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer)
                  .setSharingMode(vk::SharingMode::eExclusive)
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
          .build();

      textureBuffer = Buffer::builder()
          .setPhysicalDevice(root.physicalDevice.value)
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::BufferCreateInfo()
                  .setSize(textureWidth * textureHeight * textureDepth)
                  .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
                  .setSharingMode(vk::SharingMode::eExclusive)
          )
          .setMemoryCreateInfo(
              vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
          )
          .build();

      textureImage = Image::builder()
          .setPhysicalDevice(root.physicalDevice.value)
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::ImageCreateInfo()
                  .setImageType(vk::ImageType::e2D)
                  .setFormat(vk::Format::eR8G8B8A8Srgb)
                  .setExtent(
                      vk::Extent3D()
                          .setWidth(textureWidth)
                          .setHeight(textureHeight)
                          .setDepth(1)
                  )
                  .setMipLevels(textureMipLevels)
                  .setArrayLayers(1)
                  .setSamples(vk::SampleCountFlagBits::e1)
                  .setTiling(vk::ImageTiling::eOptimal)
                  .setUsage(vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
                  .setSharingMode(vk::SharingMode::eExclusive)
                  .setQueueFamilyIndices({})
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
          .build();

      textureImageView = ImageView::builder()
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::ImageViewCreateInfo()
                  .setImage(*textureImage.reference())
                  .setFormat(textureImage.createInfo.format)
                  .setViewType(vk::ImageViewType::e2D)
                  .setFlags({})
                  .setComponents({})
                  .setSubresourceRange(
                      vk::ImageSubresourceRange()
                          .setAspectMask(vk::ImageAspectFlagBits::eColor)
                          .setBaseMipLevel(0)
                          .setLevelCount(textureImage.createInfo.mipLevels)
                          .setBaseArrayLayer(0)
                          .setLayerCount(1)
                  )
          )
          .build();

      shadowUniformBuffers.resize(root.swapchainImageViews.size());
      for (auto& uniformBuffer : shadowUniformBuffers) {
        uniformBuffer = Buffer::builder()
            .setPhysicalDevice(root.physicalDevice.value)
            .setDevice(root.device.value)
            .setCreateInfo(
                vk::BufferCreateInfo()
                    .setSize(sizeof(UniformBufferObject) * shadowUniformBuffers.size())
                    .setUsage(vk::BufferUsageFlagBits::eUniformBuffer)
                    .setSharingMode(vk::SharingMode::eExclusive)
            )
            .setMemoryCreateInfo(
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
            )
            .build();
      }

      uniformBuffers.resize(root.swapchainImageViews.size());
      for (auto& uniformBuffer : uniformBuffers) {
        uniformBuffer = Buffer::builder()
            .setPhysicalDevice(root.physicalDevice.value)
            .setDevice(root.device.value)
            .setCreateInfo(
                vk::BufferCreateInfo()
                    .setSize(sizeof(UniformBufferObject) * uniformBuffers.size())
                    .setUsage(vk::BufferUsageFlagBits::eUniformBuffer)
                    .setSharingMode(vk::SharingMode::eExclusive)
            )
            .setMemoryCreateInfo(
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
            )
            .build();
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

}

#undef CALL_INFO
