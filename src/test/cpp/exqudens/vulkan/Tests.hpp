#pragma once

#include <cstdint>
#include <unordered_map>
#include <filesystem>
#include <chrono>
#include <functional>
#include <iostream>

#include <gtest/gtest.h>
#include <vulkan/vulkan_raii.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TestUtils.hpp"
#include "exqudens/vulkan/all.hpp"
#include "exqudens/vulkan/Vertex.hpp"
#include "exqudens/vulkan/UniformBufferObject.hpp"
#include "exqudens/vulkan/Context.hpp"
#include "exqudens/vulkan/DataContext.hpp"
#include "exqudens/vulkan/LightContext.hpp"
#include "exqudens/vulkan/CameraContext.hpp"

namespace exqudens::vulkan {

  class Tests : public testing::Test {

    protected:

      uint32_t width = 800;
      uint32_t height = 600;
      Context root = {};
      DataContext data = {};
      LightContext light = {};
      CameraContext camera = {};
      Image outputImage = {};
      size_t currentFrame = 0;

      void reCreateSwapchain(uint32_t width, uint32_t height) {
        try {
          std::cout << CALL_INFO() << " ... call" << std::endl;

          root.initSwapchain(width, height, {"resources/shader/shader-1.vert.spv", "resources/shader/shader-1.frag.spv"});

          root.device.reference().waitIdle();

          camera.initSwapchain(root, {"resources/shader/shader-2.vert.spv", "resources/shader/shader-2.frag.spv"});

          std::cout << "camera.colorImage: '" << (bool) camera.colorImage.value << "'" << std::endl;
          std::cout << "camera.colorImageView: '" << (bool) camera.colorImageView.value << "'" << std::endl;
          std::cout << "camera.depthImage: '" << (bool) camera.depthImage.value << "'" << std::endl;
          std::cout << "camera.depthImageView: '" << (bool) camera.depthImageView.value << "'" << std::endl;
          std::cout << "camera.renderPass: '" << (bool) camera.renderPass.value << "'" << std::endl;
          std::cout << "camera.pipeline: '" << (bool) camera.pipeline.value << "'" << std::endl;
          std::ranges::for_each(camera.framebuffers, [](const auto& o1) {std::cout << "camera.swapchainFramebuffer: '" << (bool) o1.value << "'" << std::endl;});

          root.transferCommandBuffer.reference().begin({});
          TestUtils::insertDepthImagePipelineBarrier(root.transferCommandBuffer, camera.depthImage);
          root.transferCommandBuffer.reference().end();
          root.transferQueue.reference().submit(
              {
                  vk::SubmitInfo()
                      .setCommandBufferCount(1)
                      .setPCommandBuffers(&(*root.transferCommandBuffer.reference()))
              }
          );
          root.transferQueue.reference().waitIdle();

          std::cout << CALL_INFO() << " ... done" << std::endl;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      void drawFrame(uint8_t& timeDiffCounter, const uint32_t& width, const uint32_t& height, const std::vector<uint16_t>& indexVector) {
        try {
          root.device.reference().resetFences({*root.inFlightFences.at(currentFrame).reference()});
          root.graphicsCommandBuffers.at(currentFrame).reference().reset();

          root.graphicsCommandBuffers.at(currentFrame).reference().begin({});
          std::vector<vk::ClearValue> clearValues = {};

          root.graphicsCommandBuffers.at(currentFrame).reference().writeTimestamp(vk::PipelineStageFlagBits::eTopOfPipe, *root.queryPool.reference(), 0);

          /*clearValues = {
              vk::ClearValue()
                  .setDepthStencil(
                      vk::ClearDepthStencilValue()
                          .setDepth(1.0f)
                          .setStencil(0)
                  )
          };

          root.graphicsCommandBuffers.at(currentFrame).reference().beginRenderPass(
              vk::RenderPassBeginInfo()
                  .setRenderPass(*camera.renderPass.reference())
                  .setFramebuffer(*camera.framebuffers.at(0).reference())
                  .setRenderArea(
                      vk::Rect2D()
                          .setOffset(
                              vk::Offset2D()
                                  .setX(0)
                                  .setY(0)
                          )
                          .setExtent(
                              vk::Extent2D()
                                  .setWidth(root.samplerImages.at(0).createInfo.extent.width)
                                  .setHeight(root.samplerImages.at(0).createInfo.extent.height)
                          )
                  )
                  .setClearValues(clearValues),
              vk::SubpassContents::eInline
          );

          root.graphicsCommandBuffers.at(currentFrame).reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *camera.pipeline.reference());
          root.graphicsCommandBuffers.at(currentFrame).reference().bindVertexBuffers(0, {*data.vertexBuffer.reference()}, {0});
          root.graphicsCommandBuffers.at(currentFrame).reference().bindIndexBuffer(*data.indexBuffer.reference(), 0, vk::IndexType::eUint16);
          root.graphicsCommandBuffers.at(currentFrame).reference().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *camera.pipeline.layoutReference(), 0, {*camera.descriptorSets.at(currentFrame).reference()}, {});
          root.graphicsCommandBuffers.at(currentFrame).reference().drawIndexed(indexVector.size(), 1, 0, 0, 0);

          root.graphicsCommandBuffers.at(currentFrame).reference().endRenderPass();

          root.graphicsCommandBuffers.at(currentFrame).reference().beginRenderPass(
              vk::RenderPassBeginInfo()
                  .setRenderPass(*root.renderPass.reference())
                  .setFramebuffer(*root.swapchainFramebuffers.at(0).reference())
                  .setRenderArea(
                      vk::Rect2D()
                          .setOffset(
                              vk::Offset2D()
                                  .setX(0)
                                  .setY(0)
                          )
                          .setExtent(root.swapchain.createInfo.imageExtent)
                  )
                  .setClearValues(clearValues),
              vk::SubpassContents::eInline
          );

          root.graphicsCommandBuffers.at(currentFrame).reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *root.pipeline.reference());
          root.graphicsCommandBuffers.at(currentFrame).reference().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *root.pipeline.layoutReference(), 0, {*root.descriptorSets.at(currentFrame).reference()}, {});
          root.graphicsCommandBuffers.at(currentFrame).reference().draw(3, 1, 0, 0);

          root.graphicsCommandBuffers.at(currentFrame).reference().endRenderPass();*/

          clearValues = {
              vk::ClearValue()
                  .setDepthStencil(
                      vk::ClearDepthStencilValue()
                          .setDepth(1.0f)
                          .setStencil(0)
                  )
          };

          root.graphicsCommandBuffers.at(currentFrame).reference().beginRenderPass(
              vk::RenderPassBeginInfo()
                  .setRenderPass(*light.renderPass.reference())
                  .setFramebuffer(*light.framebuffers.at(0).reference())
                  .setRenderArea(
                      vk::Rect2D()
                          .setOffset(
                              vk::Offset2D()
                                  .setX(0)
                                  .setY(0)
                          )
                          .setExtent(
                              vk::Extent2D()
                                  .setWidth(light.shadowImages.at(0).createInfo.extent.width)
                                  .setHeight(light.shadowImages.at(0).createInfo.extent.height)
                          )
                  )
                  .setClearValues(clearValues),
              vk::SubpassContents::eInline
          );

          root.graphicsCommandBuffers.at(currentFrame).reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *light.pipeline.reference());
          root.graphicsCommandBuffers.at(currentFrame).reference().bindVertexBuffers(0, {*data.vertexBuffer.reference()}, {0});
          root.graphicsCommandBuffers.at(currentFrame).reference().bindIndexBuffer(*data.indexBuffer.reference(), 0, vk::IndexType::eUint16);
          root.graphicsCommandBuffers.at(currentFrame).reference().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *light.pipeline.layoutReference(), 0, {*light.descriptorSets.at(currentFrame).reference()}, {});
          root.graphicsCommandBuffers.at(currentFrame).reference().drawIndexed(indexVector.size(), 1, 0, 0, 0);

          root.graphicsCommandBuffers.at(currentFrame).reference().endRenderPass();

          clearValues = {
              vk::ClearValue()
                  .setColor(
                      vk::ClearColorValue()
                          .setFloat32({0.0f, 0.0f, 0.0f, 1.0f})
                  ),
              vk::ClearValue()
                  .setDepthStencil(
                      vk::ClearDepthStencilValue()
                          .setDepth(1.0f)
                          .setStencil(0)
                  ),
              vk::ClearValue()
                  .setColor(
                      vk::ClearColorValue()
                          .setFloat32({0.0f, 0.0f, 0.0f, 1.0f})
                  )
          };

          root.graphicsCommandBuffers.at(currentFrame).reference().beginRenderPass(
              vk::RenderPassBeginInfo()
                  .setRenderPass(*camera.renderPass.reference())
                  .setFramebuffer(*camera.framebuffers.at(0).reference())
                  .setRenderArea(
                      vk::Rect2D()
                          .setOffset(
                              vk::Offset2D()
                                  .setX(0)
                                  .setY(0)
                          )
                          .setExtent(
                              vk::Extent2D()
                                  .setWidth(root.samplerImages.at(0).createInfo.extent.width)
                                  .setHeight(root.samplerImages.at(0).createInfo.extent.height)
                          )
                  )
                  .setClearValues(clearValues),
              vk::SubpassContents::eInline
          );

          root.graphicsCommandBuffers.at(currentFrame).reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *camera.pipeline.reference());
          root.graphicsCommandBuffers.at(currentFrame).reference().bindVertexBuffers(0, {*data.vertexBuffer.reference()}, {0});
          root.graphicsCommandBuffers.at(currentFrame).reference().bindIndexBuffer(*data.indexBuffer.reference(), 0, vk::IndexType::eUint16);
          root.graphicsCommandBuffers.at(currentFrame).reference().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *camera.pipeline.layoutReference(), 0, {*camera.descriptorSets.at(currentFrame).reference()}, {});
          root.graphicsCommandBuffers.at(currentFrame).reference().drawIndexed(indexVector.size(), 1, 0, 0, 0);

          root.graphicsCommandBuffers.at(currentFrame).reference().endRenderPass();

          root.graphicsCommandBuffers.at(currentFrame).reference().beginRenderPass(
              vk::RenderPassBeginInfo()
                  .setRenderPass(*root.renderPass.reference())
                  .setFramebuffer(*root.swapchainFramebuffers.at(0).reference())
                  .setRenderArea(
                      vk::Rect2D()
                          .setOffset(
                              vk::Offset2D()
                                  .setX(0)
                                  .setY(0)
                          )
                          .setExtent(root.swapchain.createInfo.imageExtent)
                  )
                  .setClearValues(clearValues),
              vk::SubpassContents::eInline
          );

          root.graphicsCommandBuffers.at(currentFrame).reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *root.pipeline.reference());
          root.graphicsCommandBuffers.at(currentFrame).reference().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *root.pipeline.layoutReference(), 0, {*root.descriptorSets.at(currentFrame).reference()}, {});
          root.graphicsCommandBuffers.at(currentFrame).reference().draw(3, 1, 0, 0);

          root.graphicsCommandBuffers.at(currentFrame).reference().endRenderPass();

          root.graphicsCommandBuffers.at(currentFrame).reference().writeTimestamp(vk::PipelineStageFlagBits::eBottomOfPipe, *root.queryPool.reference(), 1);

          try {
            std::pair<vk::Result, std::vector<uint64_t>> queryResults = root.queryPool.reference().getResults<uint64_t>(0, 1, sizeof(uint64_t) * 2, sizeof(uint64_t), vk::QueryResultFlagBits::e64);
            if (vk::Result::eNotReady == queryResults.first || vk::Result::eSuccess == queryResults.first) {
              if (vk::Result::eSuccess == queryResults.first && queryResults.second.size() > 1) {
                if (timeDiffCounter == 9) {
                  float timeDiff = root.physicalDevice.reference().getProperties().limits.timestampPeriod * ((float) queryResults.second.at(1) - queryResults.second.at(0));
                  //std::cout << "timeDiff: '" << timeDiff << "'" << std::endl;
                  timeDiffCounter = 0;
                } else {
                  timeDiffCounter++;
                }
              }
            } else {
              throw std::runtime_error("failed to 'queryPool.getResults(...)'!");
            }
          } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO() + ": exception in 'queryPool.getResults(...)'!"));
          }

          root.queryPool.reference().reset(0, 2);

          root.graphicsCommandBuffers.at(currentFrame).reference().end();

          root.graphicsQueue.reference().submit(
              {
                  vk::SubmitInfo()
                      .setCommandBufferCount(1)
                      .setPCommandBuffers(&(*root.graphicsCommandBuffers.at(currentFrame).reference()))
              }
          );
          root.graphicsQueue.reference().waitIdle();
          root.device.reference().waitIdle();
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

  };

  TEST_F(Tests, test1) {
    try {
      std::cout << CALL_INFO() << " ... call" << std::endl;

      std::vector<Vertex> vertexVector = {};
      std::vector<uint16_t> indexVector = {};
      TestUtils::readObj(
          std::filesystem::path(TestUtils::getExecutableDir()).append("resources").append("obj").append("viking_room.obj").make_preferred().string(),
          vertexVector,
          indexVector
      );

      unsigned int textureWidth, textureHeight, textureDepth, textureMipLevels;
      std::vector<unsigned char> textureVector;
      TestUtils::readPng(
          std::filesystem::path(TestUtils::getExecutableDir()).append("resources").append("png").append("viking_room.png").make_preferred().string(),
          textureWidth,
          textureHeight,
          textureDepth,
          textureVector
      );
      textureMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;

      std::vector<const char*> enabledExtensionNames;
      enabledExtensionNames.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      enabledExtensionNames.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

      std::vector<const char*> enabledLayerNames;
      //Utility::setEnvironmentVariable("VK_LAYER_PATH", TestUtils::getExecutableDir());
      //enabledLayerNames.emplace_back("VK_LAYER_KHRONOS_validation");

      root.init(enabledExtensionNames, enabledLayerNames, {}, 2, width, height, {"resources/shader/shader-1.vert.spv", "resources/shader/shader-1.frag.spv"});

      std::cout << "root.instance: '" << (bool) root.instance.value << "'" << std::endl;
      std::cout << "root.surface: '" << (bool) root.surface.value << "'" << std::endl;
      std::cout << "root.physicalDevice: '" << (bool) root.physicalDevice.value << "'" << std::endl;
      std::cout << "root.physicalDeviceTimestampPeriod: '" << root.physicalDeviceTimestampPeriod << "'" << std::endl;
      std::cout << "root.physicalDeviceMsaaSamples: '" << vk::to_string(root.physicalDeviceMsaaSamples) << "'" << std::endl;
      std::cout << "root.device: '" << (bool) root.device.value << "'" << std::endl;
      std::cout << "root.transferQueue: '" << (bool) root.transferQueue.value << "'" << std::endl;
      std::cout << "root.graphicsQueue: '" << (bool) root.graphicsQueue.value << "'" << std::endl;
      std::cout << "root.presentQueue: '" << (bool) root.presentQueue.value << "'" << std::endl;
      std::cout << "root.transferCommandPool: '" << (bool) root.transferCommandPool.value << "'" << std::endl;
      std::cout << "root.graphicsCommandPool: '" << (bool) root.graphicsCommandPool.value << "'" << std::endl;
      std::cout << "root.transferCommandBuffer: '" << (bool) root.transferCommandBuffer.value << "'" << std::endl;
      std::cout << "root.descriptorSetLayout: '" << (bool) root.descriptorSetLayout.value << "'" << std::endl;
      std::cout << "root.queryPool: '" << (bool) root.queryPool.value << "'" << std::endl;
      std::cout << "root.swapchain: '" << (bool) root.swapchain.value << "'" << std::endl;
      std::cout << "root.swapchain.images.size: '" << (root.swapchain.value ? root.swapchain.reference().getImages().size() : 0) << "'" << std::endl;
      std::ranges::for_each(root.swapchainImages, [](const auto& o1) {std::cout << "root.swapchainImage: '" << (bool) o1.value << "'" << std::endl;});
      std::ranges::for_each(root.swapchainImageViews, [](const auto& o1) {std::cout << "root.swapchainImageView: '" << (bool) o1.value << "'" << std::endl;});
      std::cout << "root.renderPass: '" << (bool) root.renderPass.value << "'" << std::endl;
      std::cout << "root.pipeline: '" << (bool) root.pipeline.value << "'" << std::endl;
      std::ranges::for_each(root.swapchainFramebuffers, [](const auto& o1) {std::cout << "root.swapchainFramebuffer: '" << (bool) o1.value << "'" << std::endl;});
      std::ranges::for_each(root.samplerImages, [](auto& o1) {std::cout << "root.samplerImage: '" << (bool) o1.value << "'" << std::endl;});
      std::ranges::for_each(root.samplerImageViews, [](auto& o1) {std::cout << "root.samplerImageView: '" << (bool) o1.value << "'" << std::endl;});
      std::cout << "root.descriptorPool: '" << (bool) root.descriptorPool.value << "'" << std::endl;
      std::cout << "root.sampler: '" << (bool) root.sampler.value << "'" << std::endl;
      std::ranges::for_each(root.descriptorSets, [](auto& o1) {std::cout << "root.descriptorSet: '" << (bool) o1.value << "'" << std::endl;});
      std::ranges::for_each(root.graphicsCommandBuffers, [](const auto& o1) {std::cout << "root.graphicsCommandBuffer: '" << (bool) o1.value << "'" << std::endl;});
      std::ranges::for_each(root.imageAvailableSemaphores, [](auto& o1) {std::cout << "root.imageAvailableSemaphore: '" << (bool) o1.value << "'" << std::endl;});
      std::ranges::for_each(root.renderFinishedSemaphores, [](auto& o1) {std::cout << "root.renderFinishedSemaphore: '" << (bool) o1.value << "'" << std::endl;});
      std::ranges::for_each(root.inFlightFences, [](auto& o1) {std::cout << "root.inFlightFence: '" << (bool) o1.value << "'" << std::endl;});

      data.init(root, vertexVector, indexVector, textureWidth, textureHeight, textureDepth, textureMipLevels);

      std::cout << "data.vertexStagingBuffer: '" << (bool) data.vertexStagingBuffer.value << "'" << std::endl;
      std::cout << "data.vertexBuffer: '" << (bool) data.vertexBuffer.value << "'" << std::endl;
      std::cout << "data.indexStagingBuffer: '" << (bool) data.indexStagingBuffer.value << "'" << std::endl;
      std::cout << "data.indexBuffer: '" << (bool) data.indexBuffer.value << "'" << std::endl;
      std::cout << "data.textureBuffer: '" << (bool) data.textureBuffer.value << "'" << std::endl;
      std::cout << "data.textureImage: '" << (bool) data.textureImage.value << "'" << std::endl;
      std::cout << "data.textureImageView: '" << (bool) data.textureImageView.value << "'" << std::endl;
      std::ranges::for_each(data.uniformBuffers, [](auto& o1) {std::cout << "data.uniformBuffer: '" << (bool) o1.value << "'" << std::endl;});

      light.init(root, data.shadowUniformBuffers, {"resources/shader/shader-3.vert.spv", "resources/shader/shader-3.frag.spv"});

      std::ranges::for_each(light.shadowImages, [](auto& o1) {std::cout << "light.shadowImage: '" << (bool) o1.value << "'" << std::endl;});
      std::ranges::for_each(light.shadowImageViews, [](auto& o1) {std::cout << "light.shadowImageView: '" << (bool) o1.value << "'" << std::endl;});
      std::cout << "light.sampler: '" << (bool) light.sampler.value << "'" << std::endl;
      std::cout << "light.descriptorSetLayout: '" << (bool) light.descriptorSetLayout.value << "'" << std::endl;
      std::cout << "light.descriptorPool: '" << (bool) light.descriptorPool.value << "'" << std::endl;
      std::cout << "light.descriptorPool: '" << (bool) light.descriptorPool.value << "'" << std::endl;
      std::ranges::for_each(light.descriptorSets, [](auto& o1) {std::cout << "light.descriptorSet: '" << (bool) o1.value << "'" << std::endl;});
      std::cout << "light.renderPass: '" << (bool) light.renderPass.value << "'" << std::endl;
      std::cout << "light.pipeline: '" << (bool) light.pipeline.value << "'" << std::endl;
      std::ranges::for_each(light.framebuffers, [](auto& o1) {std::cout << "light.framebuffer: '" << (bool) o1.value << "'" << std::endl;});

      camera.init(root, light, data.textureImage.createInfo.mipLevels, data.uniformBuffers, data.textureImageView);

      std::cout << "camera.descriptorSetLayout: '" << (bool) camera.descriptorSetLayout.value << "'" << std::endl;
      std::cout << "camera.sampler: '" << (bool) camera.sampler.value << "'" << std::endl;
      std::cout << "camera.descriptorPool: '" << (bool) camera.descriptorPool.value << "'" << std::endl;
      std::ranges::for_each(camera.descriptorSets, [](auto& o1) {std::cout << "camera.descriptorSet: '" << (bool) o1.value << "'" << std::endl;});

      data.vertexStagingBuffer.fill(vertexVector.data());
      data.indexStagingBuffer.fill(indexVector.data());
      data.textureBuffer.fill(textureVector.data());

      root.transferCommandBuffer.reference().begin({});
      root.transferCommandBuffer.reference().copyBuffer(
          *data.vertexStagingBuffer.reference(),
          *data.vertexBuffer.reference(),
          {vk::BufferCopy().setSize(data.vertexStagingBuffer.createInfo.size)}
      );
      root.transferCommandBuffer.reference().copyBuffer(
          *data.indexStagingBuffer.reference(),
          *data.indexBuffer.reference(),
          {vk::BufferCopy().setSize(data.indexStagingBuffer.createInfo.size)}
      );
      TestUtils::copyBufferToImageAndGenerateMipmaps(root.physicalDevice, root.transferCommandBuffer, data.textureBuffer, data.textureImage);
      root.transferCommandBuffer.reference().end();
      root.transferQueue.reference().submit(
          {
              vk::SubmitInfo()
                  .setCommandBufferCount(1)
                  .setPCommandBuffers(&(*root.transferCommandBuffer.reference()))
          }
      );
      root.transferQueue.reference().waitIdle();

      reCreateSwapchain(width, height);

      outputImage = Image::builder()
          .setPhysicalDevice(root.physicalDevice.value)
          .setDevice(root.device.value)
          .setCreateInfo(
              vk::ImageCreateInfo()
                  .setExtent(root.swapchainImages.at(0).createInfo.extent)
                  .setFormat(root.swapchainImages.at(0).createInfo.format)
                  .setImageType(root.swapchainImages.at(0).createInfo.imageType)
                  .setMipLevels(root.swapchainImages.at(0).createInfo.mipLevels)
                  .setArrayLayers(root.swapchainImages.at(0).createInfo.arrayLayers)
                  .setSamples(root.swapchainImages.at(0).createInfo.samples)
                  .setSharingMode(root.swapchainImages.at(0).createInfo.sharingMode)
                  .setTiling(vk::ImageTiling::eLinear)
                  .setUsage(vk::ImageUsageFlagBits::eTransferDst)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
          .build();
      std::cout << "outputImage: '" << (bool) outputImage.value << "'" << std::endl;

      float angleLeft = 0.0f;
      float angleUp = 0.0f;
      std::string animate = "";
      bool left = false;
      bool right = false;
      bool up = false;
      bool down = false;
      std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();

      TestUtils::updateUniformBuffer(
          data.shadowUniformBuffers.at(currentFrame),
          data.uniformBuffers.at(currentFrame),
          startTime,
          angleLeft,
          angleUp,
          width,
          height,
          animate,
          left,
          right,
          up,
          down
      );

      uint8_t timeDiffCounter = 9;

      drawFrame(timeDiffCounter, width, height, indexVector);

      TestUtils::copySwapchainImageToOutputImage(root.transferCommandBuffer, root.swapchainImages.at(currentFrame), outputImage, root.transferQueue);

      vk::SubresourceLayout outputImageSubresourceLayout = outputImage.reference().getSubresourceLayout(
          vk::ImageSubresource()
              .setAspectMask(vk::ImageAspectFlagBits::eColor)
      );

      unsigned char* outputImageData = (unsigned char*) outputImage.memoryReference().mapMemory(0, outputImage.reference().getMemoryRequirements().size);
      outputImageData += outputImageSubresourceLayout.offset;
      size_t outputImageHeight = outputImage.createInfo.extent.height;
      size_t outputImageWidth = outputImage.createInfo.extent.width;
      size_t outputImageDepth = 4;
      std::vector<std::vector<std::vector<unsigned char>>> outputImageVector;
      outputImageVector.resize(outputImageHeight);
      for (uint32_t y = 0; y < outputImageHeight; y++) {
        outputImageVector[y].resize(outputImageWidth);
        for (uint32_t x = 0; x < outputImageWidth; x++) {
          outputImageVector[y][x].resize(outputImageDepth);
          for (uint32_t z = 0; z < outputImageDepth; z++) {
            std::size_t yOffset = y * outputImageSubresourceLayout.rowPitch;
            std::size_t xOffset = x * outputImageDepth;
            std::size_t zOffset = z;
            std::size_t offset = yOffset + xOffset + zOffset;
            outputImageVector[y][x][z] = outputImageData[offset];
          }
        }
      }
      TestUtils::writePng(
          outputImageVector,
          std::filesystem::path(TestUtils::getExecutableDir())
              .append("resources")
              .append("png")
              .append(
                  std::string(testing::UnitTest::GetInstance()->current_test_info()->test_suite_name())
                      .append("-")
                      .append(testing::UnitTest::GetInstance()->current_test_info()->name())
                      .append("-actual.png")
              )
              .make_preferred()
              .string()
      );
      outputImage.memoryReference().unmapMemory();

      root.device.reference().waitIdle();

      std::vector<std::vector<std::vector<unsigned char>>> expected = TestUtils::readPng(
          std::filesystem::path(TestUtils::getExecutableDir())
              .append("resources")
              .append("png")
              .append(
                  std::string(testing::UnitTest::GetInstance()->current_test_info()->test_suite_name())
                      .append("-")
                      .append(testing::UnitTest::GetInstance()->current_test_info()->name())
                      .append("-expected.png")
              )
              .make_preferred()
              .string()
      );
      std::vector<std::vector<std::vector<unsigned char>>> actual = TestUtils::readPng(
          std::filesystem::path(TestUtils::getExecutableDir())
              .append("resources")
              .append("png")
              .append(
                  std::string(testing::UnitTest::GetInstance()->current_test_info()->test_suite_name())
                      .append("-")
                      .append(testing::UnitTest::GetInstance()->current_test_info()->name())
                      .append("-actual.png")
              )
              .make_preferred()
              .string()
      );
      ASSERT_EQ(expected.size(), actual.size());
      ASSERT_EQ(expected[0].size(), actual[0].size());
      ASSERT_EQ(expected[0][0].size(), actual[0][0].size());
      ASSERT_EQ(expected, actual);

      std::cout << CALL_INFO() + " ... done" << std::endl;
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
