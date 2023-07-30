#pragma once

#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <format>

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

  class UiTests : public testing::Test {

    protected:

      class TestRenderer {

        public:

          bool resized = false;
          bool left = false;
          bool right = false;
          bool up = false;
          bool down = false;

          float angleLeft = 0.0f;
          float angleUp = 0.0f;

          std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();

        private:
          inline static const std::string OBJ_FILE_PATH = ""; // "", "viking_room.obj", "object-1.obj"
          inline static const std::string PNG_FILE_PATH = ""; // "", "viking_room.png"
          inline static const std::string ANIMATE = ""; // "", "auto", "control"

          std::vector<Vertex> vertexVector = {};
          std::vector<uint16_t> indexVector = {};
          unsigned int textureWidth;
          unsigned int textureHeight;
          unsigned int textureDepth;
          std::vector<unsigned char> textureVector = {};

          Context root = {};
          DataContext data = {};
          LightContext light = {};
          CameraContext camera = {};

          size_t currentFrame = 0;
          uint8_t timeDiffCounter = 0;

        public:

          void create(
              const std::vector<std::string>& arguments,
              const std::vector<const char*>& glfwInstanceRequiredExtensions,
              GLFWwindow* window,
              uint32_t width,
              uint32_t height
          ) {
            try {
              std::cout << std::format("{} ... call", CALL_INFO()) << std::endl;

              /*vertexVector = {
                  {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                  {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                  {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                  {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},

                  {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                  {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                  {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
                  {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}
              };

              indexVector = {
                  0, 1, 2, 2, 3, 0,
                  4, 5, 6, 6, 7, 4
              };*/

              std::string objFilePath = OBJ_FILE_PATH;
              if (objFilePath.empty()) {
                objFilePath = std::filesystem::path(arguments.front())
                    .append("resources")
                    .append("obj")
                    .append("viking_room.obj")
                    .make_preferred()
                    .string();
              } else {
                objFilePath = std::filesystem::path(arguments.front())
                    .append("resources")
                    .append("obj")
                    .append(objFilePath)
                    .make_preferred().string();
              }
              TestUtils::readObj(
                  objFilePath,
                  vertexVector,
                  indexVector
              );

              std::string pngFilePath = PNG_FILE_PATH;
              if (pngFilePath.empty()) {
                pngFilePath = std::filesystem::path(arguments.front())
                    .append("resources")
                    .append("png")
                    .append("viking_room.png")
                    .make_preferred()
                    .string();
              } else {
                pngFilePath = std::filesystem::path(arguments.front())
                    .append("resources")
                    .append("png")
                    .append(pngFilePath)
                    .make_preferred()
                    .string();
              }
              unsigned int textureMipLevels;
              TestUtils::readPng(
                  pngFilePath,
                  textureWidth,
                  textureHeight,
                  textureDepth,
                  textureVector
              );
              textureMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;

              Utility::setEnvironmentVariable("VK_LAYER_PATH", arguments.front());

              std::vector<const char*> enabledExtensionNames = glfwInstanceRequiredExtensions;
              enabledExtensionNames.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
              enabledExtensionNames.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

              std::function<VkSurfaceKHR(VkInstance)> createSurfaceFunction;
              createSurfaceFunction = [&window](VkInstance instance) -> VkSurfaceKHR {
                VkSurfaceKHR vkSurface = nullptr;
                if (glfwCreateWindowSurface(instance, window, nullptr, &vkSurface) != VK_SUCCESS || vkSurface == nullptr) {
                  throw std::runtime_error(CALL_INFO() + ": failed to create surface!");
                }
                return vkSurface;
              };

              root.init(enabledExtensionNames, createSurfaceFunction, 2, width, height, {"resources/shader/shader-1.vert.spv", "resources/shader/shader-1.frag.spv"});

              std::cout << std::format("root.instance: '{}'", (bool) root.instance.value) << std::endl;
              std::cout << std::format("root.surface: '{}'", (bool) root.surface.value) << std::endl;
              std::cout << std::format("root.physicalDevice: '{}'", (bool) root.physicalDevice.value) << std::endl;
              std::cout << std::format("root.physicalDeviceTimestampPeriod: '{}'", root.physicalDeviceTimestampPeriod) << std::endl;
              std::cout << std::format("root.physicalDeviceMsaaSamples: '{}'", vk::to_string(root.physicalDeviceMsaaSamples)) << std::endl;
              std::cout << std::format("root.device: '{}'", (bool) root.device.value) << std::endl;
              std::cout << std::format("root.transferQueue: '{}'", (bool) root.transferQueue.value) << std::endl;
              std::cout << std::format("root.graphicsQueue: '{}'", (bool) root.graphicsQueue.value) << std::endl;
              std::cout << std::format("root.presentQueue: '{}'", (bool) root.presentQueue.value) << std::endl;
              std::cout << std::format("root.transferCommandPool: '{}'", (bool) root.transferCommandPool.value) << std::endl;
              std::cout << std::format("root.graphicsCommandPool: '{}'", (bool) root.graphicsCommandPool.value) << std::endl;
              std::cout << std::format("root.transferCommandBuffer: '{}'", (bool) root.transferCommandBuffer.value) << std::endl;
              std::cout << std::format("root.descriptorSetLayout: '{}'", (bool) root.descriptorSetLayout.value) << std::endl;
              std::cout << std::format("root.queryPool: '{}'", (bool) root.queryPool.value) << std::endl;
              std::cout << std::format("root.swapchain: '{}'", (bool) root.swapchain.value) << std::endl;
              std::cout << std::format("root.swapchain.images.size: '{}'", root.swapchain.reference().getImages().size()) << std::endl;
              std::ranges::for_each(root.swapchainImages, [](const auto& o1) {std::cout << std::format("root.swapchainImage: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.swapchainImageViews, [](const auto& o1) {std::cout << std::format("root.swapchainImageView: '{}'", (bool) o1.value) << std::endl;});
              std::cout << std::format("root.renderPass: '{}'", (bool) root.renderPass.value) << std::endl;
              std::cout << std::format("root.pipeline: '{}'", (bool) root.pipeline.value) << std::endl;
              std::ranges::for_each(root.swapchainFramebuffers, [](const auto& o1) {std::cout << std::format("root.swapchainFramebuffer: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.samplerImages, [](auto& o1) {std::cout << std::format("root.samplerImage: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.samplerImageViews, [](auto& o1) {std::cout << std::format("root.samplerImageView: '{}'", (bool) o1.value) << std::endl;});
              std::cout << std::format("root.descriptorPool: '{}'", (bool) root.descriptorPool.value) << std::endl;
              std::cout << std::format("root.sampler: '{}'", (bool) root.sampler.value) << std::endl;
              std::ranges::for_each(root.descriptorSets, [](auto& o1) {std::cout << std::format("root.descriptorSet: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.graphicsCommandBuffers, [](const auto& o1) {std::cout << std::format("root.graphicsCommandBuffer: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.imageAvailableSemaphores, [](auto& o1) {std::cout << std::format("root.imageAvailableSemaphore: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.renderFinishedSemaphores, [](auto& o1) {std::cout << std::format("root.renderFinishedSemaphore: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(root.inFlightFences, [](auto& o1) {std::cout << std::format("root.inFlightFence: '{}'", (bool) o1.value) << std::endl;});

              data.init(root, vertexVector, indexVector, textureWidth, textureHeight, textureDepth, textureMipLevels);

              std::cout << std::format("data.vertexStagingBuffer: '{}'", (bool) data.vertexStagingBuffer.value) << std::endl;
              std::cout << std::format("data.vertexBuffer: '{}'", (bool) data.vertexBuffer.value) << std::endl;
              std::cout << std::format("data.indexStagingBuffer: '{}'", (bool) data.indexStagingBuffer.value) << std::endl;
              std::cout << std::format("data.indexBuffer: '{}'", (bool) data.indexBuffer.value) << std::endl;
              std::cout << std::format("data.textureBuffer: '{}'", (bool) data.textureBuffer.value) << std::endl;
              std::cout << std::format("data.textureImage: '{}'", (bool) data.textureImage.value) << std::endl;
              std::cout << std::format("data.textureImageView: '{}'", (bool) data.textureImageView.value) << std::endl;
              std::ranges::for_each(data.shadowUniformBuffers, [](auto& o1) {std::cout << std::format("data.shadowUniformBuffer: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(data.uniformBuffers, [](auto& o1) {std::cout << std::format("data.uniformBuffer: '{}'", (bool) o1.value) << std::endl;});

              light.init(root, data.shadowUniformBuffers, {"resources/shader/shader-3.vert.spv", "resources/shader/shader-3.frag.spv"});

              std::ranges::for_each(light.shadowImages, [](auto& o1) {std::cout << std::format("light.shadowImage: '{}'", (bool) o1.value) << std::endl;});
              std::ranges::for_each(light.shadowImageViews, [](auto& o1) {std::cout << std::format("light.shadowImageView: '{}'", (bool) o1.value) << std::endl;});
              std::cout << std::format("light.sampler: '{}'", (bool) light.sampler.value) << std::endl;
              std::cout << std::format("light.descriptorSetLayout: '{}'", (bool) light.descriptorSetLayout.value) << std::endl;
              std::cout << std::format("light.descriptorPool: '{}'", (bool) light.descriptorPool.value) << std::endl;
              std::cout << std::format("light.descriptorPool: '{}'", (bool) light.descriptorPool.value) << std::endl;
              std::ranges::for_each(light.descriptorSets, [](auto& o1) {std::cout << std::format("light.descriptorSet: '{}'", (bool) o1.value) << std::endl;});
              std::cout << std::format("light.renderPass: '{}'", (bool) light.renderPass.value) << std::endl;
              std::cout << std::format("light.pipeline: '{}'", (bool) light.pipeline.value) << std::endl;
              std::ranges::for_each(light.framebuffers, [](auto& o1) {std::cout << std::format("light.framebuffer: '{}'", (bool) o1.value) << std::endl;});

              camera.init(root, light, data.textureImage.createInfo.mipLevels, data.uniformBuffers, data.textureImageView);

              std::cout << std::format("camera.descriptorSetLayout: '{}'", (bool) camera.descriptorSetLayout.value) << std::endl;
              std::cout << std::format("camera.sampler: '{}'", (bool) camera.sampler.value) << std::endl;
              std::cout << std::format("camera.descriptorPool: '{}'", (bool) camera.descriptorPool.value) << std::endl;
              std::ranges::for_each(camera.descriptorSets, [](auto& o1) {std::cout << std::format("camera.descriptorSet: '{}'", (bool) o1.value) << std::endl;});

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

              std::cout << std::format("{} ... done", CALL_INFO()) << std::endl;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void reCreateSwapchain(uint32_t width, uint32_t height) {
            try {
              std::cout << std::format("{} ... call", CALL_INFO()) << std::endl;

              root.initSwapchain(width, height, {"resources/shader/shader-1.vert.spv", "resources/shader/shader-1.frag.spv"});

              root.device.reference().waitIdle();

              camera.initSwapchain(root, {"resources/shader/shader-2.vert.spv", "resources/shader/shader-2.frag.spv"});

              std::cout << std::format("camera.colorImage: '{}'", (bool) camera.colorImage.value) << std::endl;
              std::cout << std::format("camera.colorImageView: '{}'", (bool) camera.colorImageView.value) << std::endl;
              std::cout << std::format("camera.depthImage: '{}'", (bool) camera.depthImage.value) << std::endl;
              std::cout << std::format("camera.depthImageView: '{}'", (bool) camera.depthImageView.value) << std::endl;
              std::cout << std::format("camera.renderPass: '{}'", (bool) camera.renderPass.value) << std::endl;
              std::cout << std::format("camera.pipeline: '{}'", (bool) camera.pipeline.value) << std::endl;
              std::ranges::for_each(camera.framebuffers, [](const auto& o1) {std::cout << std::format("camera.swapchainFramebuffer: '{}'", (bool) o1.value) << std::endl;});

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

              std::cout << std::format("{} ... done", CALL_INFO()) << std::endl;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void drawFrame(uint32_t width, uint32_t height) {
            try {
              if (
                  root.inFlightFences.size() < root.swapchainImageViews.size()
                  || root.imageAvailableSemaphores.size() < root.swapchainImageViews.size()
                  || root.graphicsCommandBuffers.size() < root.swapchainImageViews.size()
                  || root.swapchainFramebuffers.size() < root.swapchainImageViews.size()
                  || root.descriptorSets.size() < root.swapchainImageViews.size()
                  || root.imageAvailableSemaphores.size() < root.swapchainImageViews.size()
                  || root.renderFinishedSemaphores.size() < root.swapchainImageViews.size()
              ) {
                std::throw_with_nested(std::runtime_error(CALL_INFO() + ": vector.size() < root.swapchainImageViews.size() !!!"));
              }

              vk::Result result = root.device.reference().waitForFences(
                  {*root.inFlightFences.at(currentFrame).reference()},
                  true,
                  UINT64_MAX
              );
              if (vk::Result::eSuccess != result) {
                throw std::runtime_error(CALL_INFO() + ": failed to 'device.waitForFences(...)'!");
              }

              std::pair<vk::Result, uint32_t> swapchainNextImage = root.swapchain
                  .reference()
                  .acquireNextImage(UINT64_MAX, *root.imageAvailableSemaphores.at(currentFrame).reference());
              result = swapchainNextImage.first;
              if (vk::Result::eErrorOutOfDateKHR == result) {
                reCreateSwapchain(width, height);
                return;
              } else if (vk::Result::eSuccess != result && vk::Result::eSuboptimalKHR != result) {
                throw std::runtime_error("failed to 'swapChain.acquireNextImage(...)'!");
              }

              TestUtils::updateUniformBuffer(
                  data.shadowUniformBuffers.at(currentFrame),
                  data.uniformBuffers.at(currentFrame),
                  startTime,
                  angleLeft,
                  angleUp,
                  width,
                  height,
                  ANIMATE,
                  left,
                  right,
                  up,
                  down
              );
              root.device.reference().resetFences({*root.inFlightFences.at(currentFrame).reference()});
              root.graphicsCommandBuffers.at(currentFrame).reference().reset();

              root.graphicsCommandBuffers.at(currentFrame).reference().begin({});
              std::vector<vk::ClearValue> clearValues = {};

              root.graphicsCommandBuffers.at(currentFrame).reference().writeTimestamp(vk::PipelineStageFlagBits::eTopOfPipe, *root.queryPool.reference(), 0);

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
                      .setFramebuffer(*light.framebuffers.at(swapchainNextImage.second).reference())
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
                      .setFramebuffer(*camera.framebuffers.at(swapchainNextImage.second).reference())
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

              /*root.graphicsCommandBuffers.at(currentFrame).reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *camera.pipeline.reference());
              root.graphicsCommandBuffers.at(currentFrame).reference().draw(3, 1, 0, 0);*/
              root.graphicsCommandBuffers.at(currentFrame).reference().bindPipeline(vk::PipelineBindPoint::eGraphics, *camera.pipeline.reference());
              root.graphicsCommandBuffers.at(currentFrame).reference().bindVertexBuffers(0, {*data.vertexBuffer.reference()}, {0});
              root.graphicsCommandBuffers.at(currentFrame).reference().bindIndexBuffer(*data.indexBuffer.reference(), 0, vk::IndexType::eUint16);
              root.graphicsCommandBuffers.at(currentFrame).reference().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *camera.pipeline.layoutReference(), 0, {*camera.descriptorSets.at(currentFrame).reference()}, {});
              root.graphicsCommandBuffers.at(currentFrame).reference().drawIndexed(indexVector.size(), 1, 0, 0, 0);

              root.graphicsCommandBuffers.at(currentFrame).reference().endRenderPass();

              root.graphicsCommandBuffers.at(currentFrame).reference().beginRenderPass(
                  vk::RenderPassBeginInfo()
                      .setRenderPass(*root.renderPass.reference())
                      .setFramebuffer(*root.swapchainFramebuffers.at(swapchainNextImage.second).reference())
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
                      //std::cout << std::format("timeDiff: {}", timeDiff) << std::endl;
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

              std::vector<vk::PipelineStageFlags> waitDstStageMask = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
              std::vector<vk::Semaphore> waitSemaphores = {*root.imageAvailableSemaphores.at(currentFrame).reference()};
              std::vector<vk::Semaphore> signalSemaphores = {*root.renderFinishedSemaphores.at(currentFrame).reference()};
              std::vector<vk::CommandBuffer> commandBuffers = {*root.graphicsCommandBuffers.at(currentFrame).reference()};

              root.graphicsQueue.reference().submit(
                  {
                      vk::SubmitInfo()
                          .setWaitDstStageMask(waitDstStageMask)
                          .setWaitSemaphores(waitSemaphores)
                          .setSignalSemaphores(signalSemaphores)
                          .setCommandBuffers(commandBuffers)
                  },
                  *root.inFlightFences.at(currentFrame).reference()
              );

              std::vector<vk::SwapchainKHR> swapchains = {*root.swapchain.reference()};

              result = root.presentQueue.reference().presentKHR(
                  vk::PresentInfoKHR()
                      .setWaitSemaphores(signalSemaphores)
                      .setSwapchains(swapchains)
                      .setImageIndices(swapchainNextImage.second)
              );
              if (vk::Result::eErrorOutOfDateKHR == result || vk::Result::eSuboptimalKHR == result || resized) {
                resized = false;
                reCreateSwapchain(width, height);
              } else if (vk::Result::eSuccess != result) {
                throw std::runtime_error("failed to 'swapChain.acquireNextImage(...)'!");
              }

              currentFrame = (currentFrame + 1) % root.swapchainImageViews.size();
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          void waitIdle() {
            try {
              root.device.reference().waitIdle();
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

      };

      class TestUiApplication {

        public:

          std::vector<std::string> arguments = {};
          TestRenderer* renderer = nullptr;

          TestUiApplication(const int& argc, char** argv) {
            try {
              for (std::size_t i = 0; i < argc; i++) {
                arguments.emplace_back(std::string(argv[i]));
              }
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          int run() {
            try {
              int width = 800;
              int height = 600;
              std::string title = "Vulkan";

              glfwInit();
              glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
              //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

              GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

              glfwSetWindowUserPointer(window, this);
              glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
              //glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
              glfwSetKeyCallback(window, keyCallback);

              uint32_t glfwExtensionCount = 0;
              const char** glfwExtensions;
              glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
              std::vector<const char*> glfwInstanceRequiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

              renderer = new TestRenderer();
              renderer->create(
                  arguments,
                  glfwInstanceRequiredExtensions,
                  window,
                  static_cast<uint32_t>(width),
                  static_cast<uint32_t>(height)
              );

              while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
                glfwGetFramebufferSize(window, &width, &height);
                while (width == 0 || height == 0) {
                  glfwGetFramebufferSize(window, &width, &height);
                  glfwWaitEvents();
                }
                renderer->drawFrame(width, height);
              }
              renderer->waitIdle();

              delete renderer;
              glfwDestroyWindow(window);
              glfwTerminate();

              return EXIT_SUCCESS;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          static void frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
            try {
              auto* app = reinterpret_cast<TestUiApplication*>(glfwGetWindowUserPointer(window));
              app->renderer->resized = true;
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

          static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            try {
              auto* app = reinterpret_cast<TestUiApplication*>(glfwGetWindowUserPointer(window));
              int aState = glfwGetKey(window, GLFW_KEY_A);
              int leftState = glfwGetKey(window, GLFW_KEY_LEFT);
              int dState = glfwGetKey(window, GLFW_KEY_D);
              int rightState = glfwGetKey(window, GLFW_KEY_RIGHT);
              int wState = glfwGetKey(window, GLFW_KEY_W);
              int upState = glfwGetKey(window, GLFW_KEY_UP);
              int sState = glfwGetKey(window, GLFW_KEY_S);
              int downState = glfwGetKey(window, GLFW_KEY_DOWN);
              if (aState == GLFW_PRESS || leftState == GLFW_PRESS) {
                app->renderer->left = true;
                app->renderer->right = false;
                app->renderer->up = false;
                app->renderer->down = false;
              } else if (dState == GLFW_PRESS || rightState == GLFW_PRESS) {
                app->renderer->left = false;
                app->renderer->right = true;
                app->renderer->up = false;
                app->renderer->down = false;
              } else if (wState == GLFW_PRESS || upState == GLFW_PRESS) {
                app->renderer->left = false;
                app->renderer->right = false;
                app->renderer->up = true;
                app->renderer->down = false;
              } else if (sState == GLFW_PRESS || downState == GLFW_PRESS) {
                app->renderer->left = false;
                app->renderer->right = false;
                app->renderer->up = false;
                app->renderer->down = true;
              } else {
                app->renderer->left = false;
                app->renderer->right = false;
                app->renderer->up = false;
                app->renderer->down = false;
              }
            } catch (...) {
              std::throw_with_nested(std::runtime_error(CALL_INFO()));
            }
          }

      };

      void SetUp() override {
        try {
          std::cout << std::format("{}", CALL_INFO()) << std::endl;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

      void TearDown() override {
        try {
          std::cout << std::format("{}", CALL_INFO()) << std::endl;
        } catch (...) {
          std::throw_with_nested(std::runtime_error(CALL_INFO()));
        }
      }

  };

  TEST_F(UiTests, test1) {
    try {
      std::string executableDir = TestUtils::getExecutableDir();
      std::filesystem::path srcResourcesDir = std::filesystem::path(executableDir)
          .append("..")
          .append("..")
          .append("..")
          .append("..")
          .append("src")
          .append("test")
          .append("resources")
          .make_preferred();
      std::filesystem::path dstResourcesDir = std::filesystem::path(executableDir)
          .append("resources")
          .make_preferred();
      std::filesystem::path srcObjDir = std::filesystem::path(srcResourcesDir).append("obj").make_preferred();
      std::filesystem::path dstObjDir = std::filesystem::path(dstResourcesDir).append("obj").make_preferred();
      std::filesystem::copy(srcObjDir, dstObjDir, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
      std::vector<char*> arguments = {executableDir.data()};
      int argc = static_cast<int>(arguments.size());
      char** argv = &arguments[0];
      int result = TestUiApplication(argc, argv).run();
      ASSERT_EQ(EXIT_SUCCESS, result);
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
