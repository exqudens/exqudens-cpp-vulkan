#pragma once

#include <cstdint>
#include <unordered_map>
#include <filesystem>
#include <chrono>
#include <functional>
#include <format>
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
#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#endif

#include "TestUtils.hpp"
#include "exqudens/vulkan/all.hpp"
#include "exqudens/vulkan/Vertex.hpp"
#include "exqudens/vulkan/UniformBufferObject.hpp"

namespace exqudens::vulkan {

  class Tests : public testing::Test {};

  TEST_F(Tests, test1) {
    try {
      std::vector<Vertex> vertexVector = {};
      std::vector<uint16_t> indexVector = {};

      Instance instance = {};
      vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceHostQueryResetFeatures> physicalDeviceFeatures = {};
      PhysicalDevice physicalDevice = {};
      Device device = {};
      Queue transferQueue = {};
      Queue graphicsQueue = {};
      Queue presentQueue = {};
      CommandPool transferCommandPool = {};
      CommandPool graphicsCommandPool = {};
      CommandBuffer transferCommandBuffer = {};
      CommandBuffer graphicsCommandBuffer = {};
      DescriptorSetLayout descriptorSetLayout = {};
      Image swapchainImage = {};
      ImageView swapchainImageView = {};
      Image depthImage = {};
      ImageView depthImageView = {};
      Image colorImage = {};
      ImageView colorImageView = {};
      RenderPass renderPass = {};
      std::map<std::string, std::pair<vk::ShaderModuleCreateInfo, std::shared_ptr<vk::raii::ShaderModule>>> shaders;
      Pipeline pipeline = {};
      Framebuffer swapchainFramebuffer = {};
      Buffer textureBuffer = {};
      Image textureImage = {};
      ImageView textureImageView = {};
      Buffer vertexStagingBuffer = {};
      Buffer vertexBuffer = {};
      Buffer indexStagingBuffer = {};
      Buffer indexBuffer = {};
      Buffer uniformBuffer = {};
      Sampler sampler = {};
      DescriptorPool descriptorPool = {};
      DescriptorSet descriptorSet = {};
      QueryPool queryPool = {};
      Image outputImage = {};

      float physicalDeviceTimestampPeriod = 0;
      vk::SampleCountFlagBits physicalDeviceMsaaSamples = {};

      std::function<void (const std::string& path, std::vector<Vertex>& vertices, std::vector<uint16_t>& indices)> readObj;
      std::function<vk::PipelineShaderStageCreateInfo(const std::string&)> createStage;
      std::function<void ()> insertDepthImagePipelineBarrier;
      std::function<void ()> copyBufferToImageAndGenerateMipmaps;
      std::function<void()> updateUniformBuffer;
      std::function<void(Device&, CommandBuffer&, Queue&, const std::chrono::seconds&)> submitBlocking;
      std::function<void()> drawFrame;
      std::function<void()> copySwapchainImageToOutputImage;

      std::vector<std::vector<std::vector<unsigned char>>> outputImageVector;

      readObj = [](const std::string& path, std::vector<Vertex>& vertices, std::vector<uint16_t>& indices) {
        try {
          tinyobj::attrib_t attrib;
          std::vector<tinyobj::shape_t> shapes;
          std::vector<tinyobj::material_t> materials;
          std::string err;

          if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) {
            throw std::runtime_error(err);
          }

          std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

          for (const auto& shape: shapes) {
            for (const auto& index: shape.mesh.indices) {
              Vertex vertex = {};

              vertex.pos = {
                  attrib.vertices[3 * index.vertex_index + 0],
                  attrib.vertices[3 * index.vertex_index + 1],
                  attrib.vertices[3 * index.vertex_index + 2]
              };

              vertex.texCoord = {
                  attrib.texcoords[2 * index.texcoord_index + 0],
                  1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
              };

              vertex.color = {1.0f, 1.0f, 1.0f};

              if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.emplace_back(vertex);
              }

              indices.emplace_back(uniqueVertices[vertex]);
            }
          }
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      };

      createStage = [&shaders, &device](const std::string& path) {
        try {
          std::vector<char> bytes = Utility::readFile(path);
          if (bytes.empty()) {
            throw std::runtime_error(CALL_INFO() + ": '" + path + "' failed to create shader module bytes is empty!");
          }
          vk::ShaderModuleCreateInfo shaderCreateInfo = vk::ShaderModuleCreateInfo()
              .setCodeSize(bytes.size())
              .setPCode(reinterpret_cast<const uint32_t*>(bytes.data()));
          shaders[path] = std::make_pair(
              shaderCreateInfo,
              std::make_shared<vk::raii::ShaderModule>(device.reference(), shaderCreateInfo)
          );
          return vk::PipelineShaderStageCreateInfo()
              .setPName("main")
              .setModule(*(*shaders[path].second))
              .setStage(
                  path.ends_with(".vert.spv") ? vk::ShaderStageFlagBits::eVertex : vk::ShaderStageFlagBits::eFragment);
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      };

      insertDepthImagePipelineBarrier = [&transferCommandBuffer, &depthImage]() {
        try {
          transferCommandBuffer.reference().pipelineBarrier(
              vk::PipelineStageFlagBits::eTopOfPipe,
              vk::PipelineStageFlagBits::eEarlyFragmentTests,
              vk::DependencyFlags(0),
              {},
              {},
              {
                  vk::ImageMemoryBarrier()
                      .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setImage(*depthImage.reference())
                      .setOldLayout(vk::ImageLayout::eUndefined)
                      .setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
                      .setSrcAccessMask(vk::AccessFlagBits::eNoneKHR)
                      .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
                      .setSubresourceRange(
                          vk::ImageSubresourceRange()
                              .setAspectMask(
                                  depthImage.createInfo.format == vk::Format::eD32SfloatS8Uint
                                  || depthImage.createInfo.format == vk::Format::eD24UnormS8Uint
                                  ? vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil
                                  : vk::ImageAspectFlagBits::eDepth
                              )
                              .setBaseMipLevel(0)
                              .setLevelCount(1)
                              .setBaseArrayLayer(0)
                              .setLayerCount(1)
                      )
              }
          );
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      };

      copyBufferToImageAndGenerateMipmaps = [
          &physicalDevice,
          &transferCommandBuffer,
          &textureImage,
          &textureBuffer
      ]() {
        try {
          transferCommandBuffer.reference().pipelineBarrier(
              vk::PipelineStageFlagBits::eTopOfPipe,
              vk::PipelineStageFlagBits::eTransfer,
              vk::DependencyFlags(0),
              {},
              {},
              {
                  vk::ImageMemoryBarrier()
                      .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setImage(*textureImage.reference())
                      .setOldLayout(vk::ImageLayout::eUndefined)
                      .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
                      .setSrcAccessMask(vk::AccessFlagBits::eNoneKHR)
                      .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
                      .setSubresourceRange(
                          vk::ImageSubresourceRange()
                              .setAspectMask(vk::ImageAspectFlagBits::eColor)
                              .setBaseMipLevel(0)
                              .setLevelCount(textureImage.createInfo.mipLevels)
                              .setBaseArrayLayer(0)
                              .setLayerCount(1)
                      )
              }
          );

          transferCommandBuffer.reference().copyBufferToImage(
              *textureBuffer.reference(),
              *textureImage.reference(),
              vk::ImageLayout::eTransferDstOptimal,
              {
                  vk::BufferImageCopy()
                      .setBufferOffset(0)
                      .setBufferRowLength(0)
                      .setImageOffset(
                          vk::Offset3D()
                              .setX(0)
                              .setY(0)
                              .setZ(0)
                      )
                      .setImageExtent(
                          vk::Extent3D()
                              .setWidth(textureImage.createInfo.extent.width)
                              .setHeight(textureImage.createInfo.extent.height)
                              .setDepth(1)
                      )
                      .setImageSubresource(
                          vk::ImageSubresourceLayers()
                              .setAspectMask(vk::ImageAspectFlagBits::eColor)
                              .setMipLevel(0)
                              .setBaseArrayLayer(0)
                              .setLayerCount(1)
                      )
              }
          );

          // Check if image format supports linear blitting
          vk::FormatProperties formatProperties = physicalDevice.reference().getFormatProperties(textureImage.createInfo.format);

          if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
            throw std::runtime_error(CALL_INFO() + "texture image format does not support linear blitting!");
          }

          auto mipWidth = (int32_t) textureImage.createInfo.extent.width;
          auto mipHeight = (int32_t) textureImage.createInfo.extent.height;

          for (uint32_t i = 1; i < textureImage.createInfo.mipLevels; i++) {
            transferCommandBuffer.reference().pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eTransfer,
                vk::DependencyFlags(0),
                {},
                {},
                {
                    vk::ImageMemoryBarrier()
                        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                        .setImage(*textureImage.reference())
                        .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                        .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
                        .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                        .setDstAccessMask(vk::AccessFlagBits::eTransferRead)
                        .setSubresourceRange(
                            vk::ImageSubresourceRange()
                                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                .setBaseMipLevel(i - 1)
                                .setLevelCount(1)
                                .setBaseArrayLayer(0)
                                .setLayerCount(1)
                        )
                }
            );

            vk::ImageBlit blit = vk::ImageBlit()
                .setSrcOffsets({
                    vk::Offset3D()
                        .setX(0)
                        .setY(0)
                        .setZ(0),
                    vk::Offset3D()
                        .setX(mipWidth)
                        .setY(mipHeight)
                        .setZ(1)
                })
                .setSrcSubresource(
                    vk::ImageSubresourceLayers()
                        .setLayerCount(1)
                        .setLayerCount(1)
                        .setAspectMask(vk::ImageAspectFlagBits::eColor)
                        .setLayerCount(1)
                        .setMipLevel(i - 1)
                        .setBaseArrayLayer(0)
                        .setLayerCount(1)
                )
                .setDstOffsets({
                    vk::Offset3D()
                        .setX(0)
                        .setY(0)
                        .setZ(0),
                    vk::Offset3D()
                        .setX((int32_t) (mipWidth > 1 ? mipWidth / 2 : 1))
                        .setY((int32_t) (mipHeight > 1 ? mipHeight / 2 : 1))
                        .setZ((int32_t) 1)
                })
                .setDstSubresource(
                    vk::ImageSubresourceLayers()
                        .setAspectMask(vk::ImageAspectFlagBits::eColor)
                        .setMipLevel(i)
                        .setBaseArrayLayer(0)
                        .setLayerCount(1)
                );

            transferCommandBuffer.reference().blitImage(
                *textureImage.reference(),
                vk::ImageLayout::eTransferSrcOptimal,
                *textureImage.reference(),
                vk::ImageLayout::eTransferDstOptimal,
                blit,
                vk::Filter::eLinear
            );

            transferCommandBuffer.reference().pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eFragmentShader,
                vk::DependencyFlags(0),
                {},
                {},
                {
                    vk::ImageMemoryBarrier()
                        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                        .setImage(*textureImage.reference())
                        .setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
                        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                        .setSrcAccessMask(vk::AccessFlagBits::eTransferRead)
                        .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
                        .setSubresourceRange(
                            vk::ImageSubresourceRange()
                                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                .setBaseMipLevel(i - 1)
                                .setLevelCount(1)
                                .setBaseArrayLayer(0)
                                .setLayerCount(1)
                        )
                }
            );

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
          }

          transferCommandBuffer.reference().pipelineBarrier(
              vk::PipelineStageFlagBits::eTransfer,
              vk::PipelineStageFlagBits::eFragmentShader,
              vk::DependencyFlags(0),
              {},
              {},
              {
                  vk::ImageMemoryBarrier()
                      .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setImage(*textureImage.reference())
                      .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                      .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                      .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                      .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
                      .setSubresourceRange(
                          vk::ImageSubresourceRange()
                              .setAspectMask(vk::ImageAspectFlagBits::eColor)
                              .setBaseMipLevel(textureImage.createInfo.mipLevels - 1)
                              .setLevelCount(1)
                              .setBaseArrayLayer(0)
                              .setLayerCount(1)
                      )
              }
          );
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      };

      updateUniformBuffer = [&swapchainImage, &uniformBuffer]() {
        try {
          float angle = 0.5f * glm::radians(0.0f); // min 0 max 360
          UniformBufferObject ubo = {};
          ubo.model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
          ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
          ubo.proj = glm::perspective(
              glm::radians(45.0f),
              (float) swapchainImage.createInfo.extent.width / (float) swapchainImage.createInfo.extent.height,
              0.1f,
              10.0f
          );
          ubo.proj[1][1] *= -1;
          Utility::copyTo(uniformBuffer, &ubo);
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      };

      submitBlocking = [](Device& device, CommandBuffer& commandBuffer, Queue& queue, const std::chrono::seconds& timeout) {
        try {
          Fence fence = Fence::builder()
              .setDevice(device.value)
              .setCreateInfo(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled))
              .build();

          device.reference().resetFences({*fence.reference()});

          queue.reference().submit(
              {
                  vk::SubmitInfo()
                      .setCommandBufferCount(1)
                      .setPCommandBuffers(&(*commandBuffer.reference()))
              },
              *fence.reference()
          );

          uint64_t timeoutConverted = std::chrono::duration<uint64_t, std::nano>(timeout).count();
          vk::Result result = device.reference().waitForFences({*fence.reference()}, true, /*UINT64_MAX*/ timeoutConverted);

          if (vk::Result::eSuccess != result) {
            throw std::runtime_error(CALL_INFO() + ": failed to 'device.waitForFences(...)'!");
          }
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      };

      drawFrame = [
          &submitBlocking,
          &physicalDevice,
          &device,
          &graphicsQueue,
          &graphicsCommandBuffer,
          &queryPool,
          &renderPass,
          &swapchainFramebuffer,
          &swapchainImage,
          &pipeline,
          &vertexBuffer,
          &indexBuffer,
          &descriptorSet,
          &indexVector
      ]() {
        try {
          graphicsCommandBuffer.reference().reset();

          graphicsCommandBuffer.reference().begin({});
          std::vector<vk::ClearValue> clearValues = {
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
                  )
          };

          graphicsCommandBuffer.reference().writeTimestamp(vk::PipelineStageFlagBits::eTopOfPipe, *queryPool.reference(), 0);

          graphicsCommandBuffer.reference().beginRenderPass(
              vk::RenderPassBeginInfo()
                  .setRenderPass(*renderPass.reference())
                  .setFramebuffer(*swapchainFramebuffer.reference())
                  .setRenderArea(
                      vk::Rect2D()
                          .setOffset(
                              vk::Offset2D()
                                  .setX(0)
                                  .setY(0)
                          )
                          .setExtent(
                              vk::Extent2D()
                                  .setWidth(swapchainImage.createInfo.extent.width)
                                  .setHeight(swapchainImage.createInfo.extent.height)
                          )
                  )
                  .setClearValues(clearValues),
              vk::SubpassContents::eInline
          );

          graphicsCommandBuffer.reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.reference());
          graphicsCommandBuffer.reference().bindVertexBuffers(0, {*vertexBuffer.reference()}, {0});
          graphicsCommandBuffer.reference().bindIndexBuffer(*indexBuffer.reference(), 0, vk::IndexType::eUint16);
          graphicsCommandBuffer.reference().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.layoutReference(), 0, {*descriptorSet.reference()}, {});
          graphicsCommandBuffer.reference().drawIndexed(indexVector.size(), 1, 0, 0, 0);

          graphicsCommandBuffer.reference().endRenderPass();

          graphicsCommandBuffer.reference().writeTimestamp(vk::PipelineStageFlagBits::eBottomOfPipe, *queryPool.reference(), 1);

          try {
            std::pair<vk::Result, std::vector<uint64_t>> queryResults = queryPool.reference().getResults<uint64_t>(0, 1, sizeof(uint64_t) * 2, sizeof(uint64_t), vk::QueryResultFlagBits::e64);
            if (vk::Result::eNotReady == queryResults.first || vk::Result::eSuccess == queryResults.first) {
              if (vk::Result::eSuccess == queryResults.first && queryResults.second.size() > 1) {
                float timeDiff = physicalDevice.reference().getProperties().limits.timestampPeriod * ((float) queryResults.second.at(1) - queryResults.second.at(0));
                std::cout << std::format("timeDiff: {}", timeDiff) << std::endl;
              }
            } else {
              throw std::runtime_error("failed to 'queryPool.getResults(...)'!");
            }
          } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO() + ": exception in 'queryPool.getResults(...)'!"));
          }

          queryPool.reference().reset(0, 2);

          graphicsCommandBuffer.reference().end();

          submitBlocking(device, graphicsCommandBuffer, graphicsQueue, std::chrono::seconds(10));
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      };

      copySwapchainImageToOutputImage = [
          &transferCommandBuffer,
          &swapchainImage,
          &outputImage
      ]() {
        try {
          transferCommandBuffer.reference().pipelineBarrier(
              vk::PipelineStageFlagBits::eTransfer,
              vk::PipelineStageFlagBits::eTransfer,
              vk::DependencyFlags(0),
              {},
              {},
              {
                  vk::ImageMemoryBarrier()
                      .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setImage(*outputImage.reference())
                      .setOldLayout(vk::ImageLayout::eUndefined)
                      .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
                      .setSrcAccessMask(vk::AccessFlagBits::eNone)
                      .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
                      .setSubresourceRange(
                          vk::ImageSubresourceRange()
                              .setAspectMask(vk::ImageAspectFlagBits::eColor)
                              .setBaseMipLevel(0)
                              .setLevelCount(1)
                              .setBaseArrayLayer(0)
                              .setLayerCount(1)
                      )
              }
          );

          transferCommandBuffer.reference().copyImage(
              *swapchainImage.reference(),
              vk::ImageLayout::eTransferSrcOptimal,
              *outputImage.reference(),
              vk::ImageLayout::eTransferDstOptimal,
              {
                  vk::ImageCopy()
                      .setExtent(outputImage.createInfo.extent)
                      .setSrcSubresource(
                          vk::ImageSubresourceLayers()
                              .setAspectMask(vk::ImageAspectFlagBits::eColor)
                              .setLayerCount(1)
                      )
                      .setDstSubresource(
                          vk::ImageSubresourceLayers()
                              .setAspectMask(vk::ImageAspectFlagBits::eColor)
                              .setLayerCount(1)
                      )
              }
          );

          transferCommandBuffer.reference().pipelineBarrier(
              vk::PipelineStageFlagBits::eTransfer,
              vk::PipelineStageFlagBits::eTransfer,
              vk::DependencyFlags(0),
              {},
              {},
              {
                  vk::ImageMemoryBarrier()
                      .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                      .setImage(*outputImage.reference())
                      .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                      .setNewLayout(vk::ImageLayout::eGeneral)
                      .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                      .setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
                      .setSubresourceRange(
                          vk::ImageSubresourceRange()
                              .setAspectMask(vk::ImageAspectFlagBits::eColor)
                              .setBaseMipLevel(0)
                              .setLevelCount(1)
                              .setBaseArrayLayer(0)
                              .setLayerCount(1)
                      )
              }
          );
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      };

      readObj(
          std::filesystem::path(TestUtils::getExecutableDir()).append("resources").append("obj").append("viking_room.obj").make_preferred().string(),
          vertexVector,
          indexVector
      );

      unsigned int tmpImageWidth, tmpImageHeight, tmpImageDepth, tmpImageMipLevels;
      std::vector<unsigned char> tmpImageData;
      TestUtils::readPng(
          std::filesystem::path(TestUtils::getExecutableDir()).append("resources").append("png").append("viking_room.png").make_preferred().string(),
          tmpImageWidth,
          tmpImageHeight,
          tmpImageDepth,
          tmpImageData
      );
      tmpImageMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(tmpImageWidth, tmpImageHeight)))) + 1;

      Utility::setEnvironmentVariable("VK_LAYER_PATH", TestUtils::getExecutableDir());

      std::vector<const char*> enabledExtensionNames;
      enabledExtensionNames.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      enabledExtensionNames.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

      instance = Instance::builder()
          .addEnabledLayerName("VK_LAYER_KHRONOS_validation")
          .setEnabledExtensionNames(enabledExtensionNames)
          .setApplicationInfo(
              vk::ApplicationInfo()
                  .setPApplicationName("Exqudens Application")
                  .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
                  .setPEngineName("Exqudens Engine")
                  .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
                  .setApiVersion(VK_API_VERSION_1_0)
          )
          .setMessengerCreateInfo(
              MessengerCreateInfo()
                  .setExceptionSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
                  //.setOutSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
                  .setToStringFunction(&Utility::toString)
          )
          .setDebugUtilsMessengerCreateInfo(
              vk::DebugUtilsMessengerCreateInfoEXT()
                  .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
                  .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
          )
      .build();
      std::cout << std::format("instance: '{}'", (bool) instance.value) << std::endl;

      vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceHostQueryResetFeatures> tmpPhysicalDeviceFeatures = {
          vk::PhysicalDeviceFeatures2()
              .setFeatures(
                  vk::PhysicalDeviceFeatures()
                      .setSamplerAnisotropy(true)
                      .setSampleRateShading(true)
              ),
          vk::PhysicalDeviceHostQueryResetFeatures()
              .setHostQueryReset(true)
      };
      physicalDeviceFeatures = tmpPhysicalDeviceFeatures;
      physicalDevice = PhysicalDevice::builder()
          .setInstance(instance.value)
          .addEnabledExtensionName(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME)
          .addQueuePriority(1.0f)
          .setIsSuitableFunction([this](const vk::raii::PhysicalDevice& p) {
            bool result = false;
            std::vector<vk::QueueFamilyProperties> properties = p.getQueueFamilyProperties();
            bool computeValid = false;
            bool transferValid = false;
            bool graphicsValid = false;
            for (size_t i = 0; i < properties.size(); i++) {
              if (properties[i].queueFlags & vk::QueueFlagBits::eCompute) {
                computeValid = true;
              }
              if (properties[i].queueFlags & vk::QueueFlagBits::eTransfer) {
                transferValid = true;
              }
              if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics && properties[i].timestampValidBits > 0) {
                graphicsValid = true;
              }

              result = computeValid && transferValid && graphicsValid;
              if (result) {
                break;
              }
            }
            if (!result) {
              return result;
            }
            auto f = p.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceHostQueryResetFeatures>();
            result = f.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy;
            if (!result) {
              return result;
            }
            result = f.get<vk::PhysicalDeviceFeatures2>().features.sampleRateShading;
            if (!result) {
              return result;
            }
            result = f.get<vk::PhysicalDeviceHostQueryResetFeatures>().hostQueryReset;
            if (!result) {
              return result;
            }
            return result;
          })
      .build();
      std::cout << std::format("physicalDevice: '{}'", (bool) physicalDevice.value) << std::endl;

      physicalDeviceTimestampPeriod = physicalDevice.reference().getProperties().limits.timestampPeriod;
      std::cout << std::format("physicalDeviceTimestampPeriod: '{}'", physicalDeviceTimestampPeriod) << std::endl;

      physicalDeviceMsaaSamples = Utility::getMaxUsableSampleCount({
          physicalDevice.reference().getProperties().limits.framebufferColorSampleCounts,
          physicalDevice.reference().getProperties().limits.framebufferDepthSampleCounts
      });
      std::cout << std::format("physicalDeviceMsaaSamples: '{}'", vk::to_string(physicalDeviceMsaaSamples)) << std::endl;

      device = Device::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setCreateInfo(
              vk::DeviceCreateInfo()
                  .setQueueCreateInfos(physicalDevice.uniqueQueueCreateInfos)
                  //.setPEnabledFeatures(&physicalDevice.features)
                  .setPEnabledExtensionNames(physicalDevice.enabledExtensionNames)
                  .setPEnabledLayerNames(instance.enabledLayerNames)
                  //.setPNext(physicalDevice.hostQueryResetFeatures.has_value() ? &physicalDevice.hostQueryResetFeatures.value() : nullptr)
                  .setPNext(&physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>())
          )
      .build();
      std::cout << std::format("device: '{}'", (bool) device.value) << std::endl;

      transferQueue = Queue::builder()
          .setDevice(device.value)
          .setFamilyIndex(physicalDevice.transferQueueCreateInfos.front().queueFamilyIndex)
      .build();
      std::cout << std::format("transferQueue: '{}'", (bool) transferQueue.value) << std::endl;
      graphicsQueue = Queue::builder()
          .setDevice(device.value)
          .setFamilyIndex(physicalDevice.graphicsQueueCreateInfos.front().queueFamilyIndex)
      .build();
      std::cout << std::format("graphicsQueue: '{}'", (bool) graphicsQueue.value) << std::endl;

      transferCommandPool = CommandPool::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::CommandPoolCreateInfo()
                  .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                  .setQueueFamilyIndex(transferQueue.familyIndex)
          )
      .build();
      std::cout << std::format("transferCommandPool: '{}'", (bool) transferCommandPool.value) << std::endl;
      graphicsCommandPool = CommandPool::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::CommandPoolCreateInfo()
                  .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                  .setQueueFamilyIndex(graphicsQueue.familyIndex)
          )
      .build();
      std::cout << std::format("graphicsCommandPool: '{}'", (bool) graphicsCommandPool.value) << std::endl;

      transferCommandBuffer = CommandBuffer::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::CommandBufferAllocateInfo()
                  .setCommandPool(*transferCommandPool.reference())
                  .setCommandBufferCount(1)
                  .setLevel(vk::CommandBufferLevel::ePrimary)
          )
      .build();
      std::cout << std::format("transferCommandBuffer: '{}'", (bool) transferCommandBuffer.value) << std::endl;
      graphicsCommandBuffer = CommandBuffer::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::CommandBufferAllocateInfo()
                  .setCommandPool(*graphicsCommandPool.reference())
                  .setCommandBufferCount(1)
                  .setLevel(vk::CommandBufferLevel::ePrimary)
          )
      .build();
      std::cout << std::format("graphicsCommandBuffer: '{}'", (bool) graphicsCommandBuffer.value) << std::endl;

      descriptorSetLayout = DescriptorSetLayout::builder()
          .setDevice(device.value)
          .addBinding(
              vk::DescriptorSetLayoutBinding()
                  .setBinding(0)
                  .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                  .setDescriptorCount(1)
                  .setStageFlags(vk::ShaderStageFlagBits::eVertex)
          )
          .addBinding(
              vk::DescriptorSetLayoutBinding()
                  .setBinding(1)
                  .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                  .setDescriptorCount(1)
                  .setStageFlags(vk::ShaderStageFlagBits::eFragment)
          )
      .build();
      std::cout << std::format("descriptorSetLayout: '{}'", (bool) descriptorSetLayout.value) << std::endl;

      swapchainImage = Image::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setDevice(device.value)
          .setCreateInfo(
              vk::ImageCreateInfo()
                  .setExtent(
                      vk::Extent3D()
                          .setWidth(800)
                          .setHeight(600)
                          .setDepth(1)
                  )
                  .setFormat(vk::Format::eR8G8B8A8Unorm)
                  .setImageType(vk::ImageType::e2D)
                  .setMipLevels(1)
                  .setArrayLayers(1)
                  .setSamples(vk::SampleCountFlagBits::e1)
                  .setTiling(vk::ImageTiling::eOptimal)
                  .setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc)
                  .setSharingMode(vk::SharingMode::eExclusive)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
          .build();
      std::cout << std::format("swapchainImage: '{}'", (bool) swapchainImage.value) << std::endl;

      swapchainImageView = ImageView::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::ImageViewCreateInfo()
                  .setImage(*swapchainImage.reference())
                  .setFormat(swapchainImage.createInfo.format)
                  .setViewType(vk::ImageViewType::e2D)
                  .setSubresourceRange(
                      vk::ImageSubresourceRange()
                          .setAspectMask(vk::ImageAspectFlagBits::eColor)
                          .setBaseMipLevel(0)
                          .setLevelCount(1)
                          .setBaseArrayLayer(0)
                          .setLayerCount(1)
                  )
          )
          .build();
      std::cout << std::format("swapchainImageView: '{}'", (bool) swapchainImageView.value) << std::endl;

      depthImage = Image::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setDevice(device.value)
          .setCreateInfo(
              vk::ImageCreateInfo()
                  .setImageType(vk::ImageType::e2D)
                  .setFormat(Utility::imageDepthFormat(physicalDevice.reference()))
                  .setExtent(
                      vk::Extent3D()
                          .setWidth(swapchainImage.createInfo.extent.width)
                          .setHeight(swapchainImage.createInfo.extent.height)
                          .setDepth(1)
                  )
                  .setMipLevels(1)
                  .setArrayLayers(1)
                  .setSamples(physicalDeviceMsaaSamples)
                  .setTiling(vk::ImageTiling::eOptimal)
                  .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
                  .setSharingMode(vk::SharingMode::eExclusive)
                  .setQueueFamilyIndices({})
                  .setInitialLayout(vk::ImageLayout::eUndefined)
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
          .build();
      std::cout << std::format("depthImage: '{}'", (bool) depthImage.value) << std::endl;

      depthImageView = ImageView::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::ImageViewCreateInfo()
                  .setImage(*depthImage.reference())
                  .setFormat(depthImage.createInfo.format)
                  .setViewType(vk::ImageViewType::e2D)
                  .setFlags({})
                  .setComponents({})
                  .setSubresourceRange(
                      vk::ImageSubresourceRange()
                          .setAspectMask(vk::ImageAspectFlagBits::eDepth)
                          .setBaseMipLevel(0)
                          .setLevelCount(1)
                          .setBaseArrayLayer(0)
                          .setLayerCount(1)
                  )
          )
          .build();
      std::cout << std::format("depthImageView: '{}'", (bool) depthImageView.value) << std::endl;

      colorImage = Image::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setDevice(device.value)
          .setCreateInfo(
              vk::ImageCreateInfo()
                  .setImageType(vk::ImageType::e2D)
                  .setFormat(swapchainImage.createInfo.format)
                  .setExtent(
                      vk::Extent3D()
                          .setWidth(swapchainImage.createInfo.extent.width)
                          .setHeight(swapchainImage.createInfo.extent.height)
                          .setDepth(1)
                  )
                  .setMipLevels(1)
                  .setArrayLayers(1)
                  .setSamples(physicalDeviceMsaaSamples)
                  .setTiling(vk::ImageTiling::eOptimal)
                  .setUsage(vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment)
                  .setSharingMode(vk::SharingMode::eExclusive)
                  .setQueueFamilyIndices({})
                  .setInitialLayout(vk::ImageLayout::eUndefined)
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
          .build();
      std::cout << std::format("colorImage: '{}'", (bool) colorImage.value) << std::endl;

      colorImageView = ImageView::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::ImageViewCreateInfo()
                  .setImage(*colorImage.reference())
                  .setFormat(colorImage.createInfo.format)
                  .setViewType(vk::ImageViewType::e2D)
                  .setFlags({})
                  .setComponents({})
                  .setSubresourceRange(
                      vk::ImageSubresourceRange()
                          .setAspectMask(vk::ImageAspectFlagBits::eColor)
                          .setBaseMipLevel(0)
                          .setLevelCount(1)
                          .setBaseArrayLayer(0)
                          .setLayerCount(1)
                  )
          )
          .build();
      std::cout << std::format("colorImageView: '{}'", (bool) colorImageView.value) << std::endl;

      renderPass = RenderPass::builder()
          .setDevice(device.value)
          .addAttachment(
              vk::AttachmentDescription()
                  .setFormat(swapchainImage.createInfo.format)
                  .setSamples(physicalDeviceMsaaSamples)
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal)
          )
          .addAttachment(
              vk::AttachmentDescription()
                  .setFormat(depthImage.createInfo.format)
                  .setSamples(physicalDeviceMsaaSamples)
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStencilLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
          )
          .addAttachment(
              vk::AttachmentDescription()
                  .setFormat(swapchainImage.createInfo.format)
                  .setSamples(vk::SampleCountFlagBits::e1)
                  .setLoadOp(vk::AttachmentLoadOp::eDontCare)
                  .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::eTransferSrcOptimal)
          )
          .addSubpass(
              SubpassDescription()
                  .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                  .addColorAttachment(
                      vk::AttachmentReference()
                          .setAttachment(0)
                          .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
                  )
                  .setDepthStencilAttachment(
                      vk::AttachmentReference()
                          .setAttachment(1)
                          .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
                  )
                  .addResolveAttachment(
                      vk::AttachmentReference()
                          .setAttachment(2)
                          .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
                  )
          )
          .addDependency(
              vk::SubpassDependency()
                  .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                  .setDstSubpass(0)
                  .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
                  .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
                  .setSrcAccessMask(vk::AccessFlagBits::eNoneKHR)
                  .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
          )
          .build();
      std::cout << std::format("renderPass: '{}'", (bool) renderPass.value) << std::endl;

      pipeline = Pipeline::builder()
          .setDevice(device.value)
          .addStage(createStage("resources/shader/shader-4.vert.spv"))
          .addPath("resources/shader/shader-4.frag.spv")
          .addSetLayout(*descriptorSetLayout.reference())
          .setGraphicsCreateInfo(
              GraphicsPipelineCreateInfo()
                  .setRenderPass(*renderPass.reference())
                  .setSubpass(0)
                  .setVertexInputState(
                      PipelineVertexInputStateCreateInfo()
                          .setVertexBindingDescriptions({Vertex::getBindingDescription()})
                          .setVertexAttributeDescriptions(Vertex::getAttributeDescriptions())
                  )
                  .setInputAssemblyState(
                      vk::PipelineInputAssemblyStateCreateInfo()
                          .setTopology(vk::PrimitiveTopology::eTriangleList)
                          .setPrimitiveRestartEnable(false)
                  )
                  .setViewportState(
                      PipelineViewportStateCreateInfo()
                          .addViewport(
                              vk::Viewport()
                                  .setWidth((float) swapchainImage.createInfo.extent.width)
                                  .setHeight((float) swapchainImage.createInfo.extent.height)
                                  .setMinDepth(0.0)
                                  .setMaxDepth(1.0)
                                  .setX(0.0)
                                  .setY(0.0)
                          )
                          .addScissor(
                              vk::Rect2D()
                                  .setOffset({0, 0})
                                  .setExtent(
                                      vk::Extent2D()
                                          .setHeight(swapchainImage.createInfo.extent.height)
                                          .setWidth(swapchainImage.createInfo.extent.width)
                                  )
                          )
                  )
                  .setRasterizationState(
                      vk::PipelineRasterizationStateCreateInfo()
                          .setDepthClampEnable(false)
                          .setRasterizerDiscardEnable(false)
                          .setPolygonMode(vk::PolygonMode::eFill)
                          .setCullMode(vk::CullModeFlagBits::eBack)
                          .setFrontFace(vk::FrontFace::eCounterClockwise)
                          .setLineWidth(1.0)
                          .setDepthBiasEnable(false)
                          .setDepthBiasConstantFactor(0.0)
                          .setDepthBiasClamp(0.0)
                          .setDepthBiasSlopeFactor(0.0)
                  )
                  .setMultisampleState(
                      vk::PipelineMultisampleStateCreateInfo()
                          .setRasterizationSamples(physicalDeviceMsaaSamples)
                          .setSampleShadingEnable(false)
                          .setMinSampleShading(1.0)
                              //.setSampleShadingEnable(true)
                              //.setMinSampleShading(0.2f)
                          .setPSampleMask(nullptr)
                          .setAlphaToCoverageEnable(false)
                          .setAlphaToOneEnable(false)
                  )
                  .setDepthStencilState(
                      vk::PipelineDepthStencilStateCreateInfo()
                          .setDepthTestEnable(true)
                          .setDepthWriteEnable(true)
                          .setDepthCompareOp(vk::CompareOp::eLess)
                          .setDepthBoundsTestEnable(false)
                          .setStencilTestEnable(false)
                          .setFront({})
                          .setBack({})
                          .setMinDepthBounds(0.0)
                          .setMaxDepthBounds(1.0)
                  )
                  .setColorBlendState(
                      PipelineColorBlendStateCreateInfo()
                          .setLogicOpEnable(false)
                          .setLogicOp(vk::LogicOp::eCopy)
                          .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f})
                          .addAttachment(
                              vk::PipelineColorBlendAttachmentState()
                                  .setBlendEnable(false)
                                  .setColorBlendOp(vk::BlendOp::eAdd)
                                  .setSrcColorBlendFactor(vk::BlendFactor::eOne)
                                  .setDstColorBlendFactor(vk::BlendFactor::eZero)
                                  .setAlphaBlendOp(vk::BlendOp::eAdd)
                                  .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
                                  .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
                                  .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
                          )
                  )
          )
          .build();
      std::cout << std::format("pipeline: '{}'", (bool) pipeline.value) << std::endl;

      swapchainFramebuffer = Framebuffer::builder()
          .setDevice(device.value)
          .addAttachment(*colorImageView.reference())
          .addAttachment(*depthImageView.reference())
          .addAttachment(*swapchainImageView.reference())
          .setCreateInfo(
              vk::FramebufferCreateInfo()
                  .setRenderPass(*renderPass.reference())
                  .setWidth(swapchainImage.createInfo.extent.width)
                  .setHeight(swapchainImage.createInfo.extent.height)
                  .setLayers(1)
          )
          .build();
      std::cout << std::format("swapchainFramebuffer: '{}'", (bool) swapchainFramebuffer.value) << std::endl;

      textureBuffer = Buffer::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setDevice(device.value)
          .setCreateInfo(
              vk::BufferCreateInfo()
                  .setSize(tmpImageWidth * tmpImageHeight * tmpImageDepth)
                  .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
                  .setSharingMode(vk::SharingMode::eExclusive)
          )
          .setMemoryCreateInfo(
              vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
          )
      .build();
      std::cout << std::format("textureBuffer: '{}'", (bool) textureBuffer.value) << std::endl;
      textureImage = Image::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setDevice(device.value)
          .setCreateInfo(
              vk::ImageCreateInfo()
                  .setImageType(vk::ImageType::e2D)
                  .setFormat(vk::Format::eR8G8B8A8Srgb)
                  .setExtent(
                      vk::Extent3D()
                          .setWidth(tmpImageWidth)
                          .setHeight(tmpImageHeight)
                          .setDepth(1)
                  )
                  .setMipLevels(tmpImageMipLevels)
                  .setArrayLayers(1)
                  .setSamples(vk::SampleCountFlagBits::e1)
                  .setTiling(vk::ImageTiling::eOptimal)
                  .setUsage(vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
                  .setSharingMode(vk::SharingMode::eExclusive)
                  .setQueueFamilyIndices({})
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
      .build();
      std::cout << std::format("textureImage: '{}'", (bool) textureImage.value) << std::endl;
      textureImageView = ImageView::builder()
          .setDevice(device.value)
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
      std::cout << std::format("textureImageView: '{}'", (bool) textureImageView.value) << std::endl;

      vertexStagingBuffer = Buffer::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setDevice(device.value)
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
      std::cout << std::format("vertexStagingBuffer: '{}'", (bool) vertexStagingBuffer.value) << std::endl;
      vertexBuffer = Buffer::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setDevice(device.value)
          .setCreateInfo(
              vk::BufferCreateInfo()
                  .setSize(vertexStagingBuffer.createInfo.size)
                  .setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer)
                  .setSharingMode(vk::SharingMode::eExclusive)
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
      .build();
      std::cout << std::format("vertexBuffer: '{}'", (bool) vertexBuffer.value) << std::endl;

      indexStagingBuffer = Buffer::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setDevice(device.value)
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
      std::cout << std::format("indexStagingBuffer: '{}'", (bool) indexStagingBuffer.value) << std::endl;
      indexBuffer = Buffer::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setDevice(device.value)
          .setCreateInfo(
              vk::BufferCreateInfo()
                  .setSize(indexStagingBuffer.createInfo.size)
                  .setUsage(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer)
                  .setSharingMode(vk::SharingMode::eExclusive)
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eDeviceLocal)
      .build();
      std::cout << std::format("indexBuffer: '{}'", (bool) indexBuffer.value) << std::endl;

      uniformBuffer = Buffer::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setDevice(device.value)
          .setCreateInfo(
              vk::BufferCreateInfo()
                  .setSize(sizeof(UniformBufferObject) * /*MAX_FRAMES_IN_FLIGHT*/ /*uniformBuffers.size()*/ 1)
                  .setUsage(vk::BufferUsageFlagBits::eUniformBuffer)
                  .setSharingMode(vk::SharingMode::eExclusive)
          )
          .setMemoryCreateInfo(
              vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
          )
      .build();
      std::cout << std::format("uniformBuffer: '{}'", (bool) uniformBuffer.value) << std::endl;

      sampler = Sampler::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::SamplerCreateInfo()
                  .setMagFilter(vk::Filter::eLinear)
                  .setMinFilter(vk::Filter::eLinear)
                  .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                  .setMinLod(0.0f)
                  //.setMinLod((float) textureImage.createInfo.mipLevels / 2)
                  .setMaxLod((float) textureImage.createInfo.mipLevels)
                  .setMipLodBias(0.0f)
                  .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                  .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                  .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                  .setCompareOp(vk::CompareOp::eAlways)
                  .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                  .setUnnormalizedCoordinates(false)
                  .setCompareEnable(false)
                  .setAnisotropyEnable(physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy)
                  .setMaxAnisotropy(physicalDeviceFeatures.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy ? physicalDevice.reference().getProperties().limits.maxSamplerAnisotropy : 0)
          )
      .build();
      std::cout << std::format("sampler: '{}'", (bool) sampler.value) << std::endl;

      descriptorPool = DescriptorPool::builder()
          .setDevice(device.value)
          .addPoolSize(
              vk::DescriptorPoolSize()
                  .setType(vk::DescriptorType::eUniformBuffer)
                  .setDescriptorCount(/*MAX_FRAMES_IN_FLIGHT*/ 1)
          )
          .addPoolSize(
              vk::DescriptorPoolSize()
                  .setType(vk::DescriptorType::eCombinedImageSampler)
                  .setDescriptorCount(/*MAX_FRAMES_IN_FLIGHT*/ 1)
          )
          .setCreateInfo(
              vk::DescriptorPoolCreateInfo()
                  .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
                  .setMaxSets(/*MAX_FRAMES_IN_FLIGHT*/ 1)
          )
      .build();
      std::cout << std::format("descriptorPool: '{}'", (bool) descriptorPool.value) << std::endl;

      descriptorSet = DescriptorSet::builder()
          .setDevice(device.value)
          .addSetLayout(*descriptorSetLayout.reference())
          .setCreateInfo(
              vk::DescriptorSetAllocateInfo()
                  .setDescriptorPool(*descriptorPool.reference())
                  .setDescriptorSetCount(1)
          )
          .addWrite(
              WriteDescriptorSet()
                  .setDstBinding(0)
                  .setDstArrayElement(0)
                  .setDescriptorCount(1)
                  .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                  .addBufferInfo(
                      vk::DescriptorBufferInfo()
                          .setBuffer(*uniformBuffer.reference())
                          .setOffset(0)
                          .setRange(sizeof(UniformBufferObject))
                  )
          )
          .addWrite(
              WriteDescriptorSet()
                  .setDstBinding(1)
                  .setDstArrayElement(0)
                  .setDescriptorCount(1)
                  .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                  .addImageInfo(
                      vk::DescriptorImageInfo()
                          .setSampler(*sampler.reference())
                          .setImageView(*textureImageView.reference())
                          .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                  )
          )
      .build();
      std::cout << std::format("descriptorSet: '{}'", (bool) descriptorSet.value) << std::endl;

      queryPool = QueryPool::builder()
          .setDevice(device.value)
          .setCreateInfo(
              vk::QueryPoolCreateInfo()
                  /*.setPipelineStatistics(vk::QueryPipelineStatisticFlagBits::eVertexShaderInvocations)*/
                  .setQueryType(vk::QueryType::eTimestamp)
                  .setQueryCount(2)
          )
      .build();
      std::cout << std::format("queryPool: '{}'", (bool) queryPool.value) << std::endl;

      outputImage = Image::builder()
          .setPhysicalDevice(physicalDevice.value)
          .setDevice(device.value)
          .setCreateInfo(
              vk::ImageCreateInfo()
                  .setExtent(swapchainImage.createInfo.extent)
                  .setFormat(swapchainImage.createInfo.format)
                  .setImageType(swapchainImage.createInfo.imageType)
                  .setMipLevels(swapchainImage.createInfo.mipLevels)
                  .setArrayLayers(swapchainImage.createInfo.arrayLayers)
                  .setSamples(swapchainImage.createInfo.samples)
                  .setSharingMode(swapchainImage.createInfo.sharingMode)
                  .setTiling(vk::ImageTiling::eLinear)
                  .setUsage(vk::ImageUsageFlagBits::eTransferDst)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
          )
          .setMemoryCreateInfo(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
      .build();
      std::cout << std::format("outputImage: '{}'", (bool) outputImage.value) << std::endl;

      Utility::copyTo(textureBuffer, tmpImageData.data());
      Utility::copyTo(vertexStagingBuffer, vertexVector.data());
      Utility::copyTo(indexStagingBuffer, indexVector.data());

      transferCommandBuffer.reference().begin({});

      insertDepthImagePipelineBarrier();

      copyBufferToImageAndGenerateMipmaps();

      transferCommandBuffer.reference().copyBuffer(
          *vertexStagingBuffer.reference(),
          *vertexBuffer.reference(),
          {
              vk::BufferCopy()
                  .setSize(vertexStagingBuffer.createInfo.size)
          }
      );

      transferCommandBuffer.reference().copyBuffer(
          *indexStagingBuffer.reference(),
          *indexBuffer.reference(),
          {
              vk::BufferCopy()
                  .setSize(indexStagingBuffer.createInfo.size)
          }
      );

      transferCommandBuffer.reference().end();
      submitBlocking(device, transferCommandBuffer, transferQueue, std::chrono::seconds(10));

      updateUniformBuffer();

      drawFrame();

      device.reference().waitIdle();

      transferCommandBuffer.reference().begin({});

      copySwapchainImageToOutputImage();

      transferCommandBuffer.reference().end();
      submitBlocking(device, transferCommandBuffer, transferQueue, std::chrono::seconds(10));

      vk::SubresourceLayout outputImageSubresourceLayout = outputImage.reference().getSubresourceLayout(
          vk::ImageSubresource()
              .setAspectMask(vk::ImageAspectFlagBits::eColor)
      );

      unsigned char* outputImageData = (unsigned char*) outputImage.memoryReference().mapMemory(0, outputImage.reference().getMemoryRequirements().size);
      outputImageData += outputImageSubresourceLayout.offset;
      size_t outputImageHeight = outputImage.createInfo.extent.height;
      size_t outputImageWidth = outputImage.createInfo.extent.width;
      size_t outputImageDepth = 4;
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

      device.reference().waitIdle();

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
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
